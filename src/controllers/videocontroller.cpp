// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videocontroller.h"

#include "config.h"
#include "general_logging.h"
#include "plasmatube.h"

#ifdef HAS_DBUS
#include <KLocalizedString>
#include <QDBusConnection>
#include <QDBusReply>
#include <QGuiApplication>
#endif

#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif

VideoController::VideoController(QObject *parent)
    : QObject(parent)
    , m_videoModel(new VideoModel(this))
    , m_videoQueue(new VideoQueue(this))
    , m_mpris(new Mpris2(this))
    , m_sponsorBlockController(new SponsorBlockController(this))
{
    connect(m_videoModel, &VideoModel::audioUrlChanged, this, [this] {
        m_currentPlayer->setAudioUrl(m_videoModel->audioUrl());
    });
    connect(m_videoModel, &VideoModel::remoteUrlChanged, this, [this] {
        Q_EMIT m_currentPlayer->command(QStringList() << QStringLiteral("loadfile") << m_videoModel->remoteUrl());
    });
    connect(m_videoModel, &VideoModel::videoChanged, this, [this] {
        Q_EMIT currentVideoChanged();
    });
    connect(m_videoQueue, &VideoQueue::currentVideoChanged, this, [this] {
        // If the user is attempting to "play" the same video, they probably meant to open the player
        if (currentVideo() && currentVideo()->videoId() == m_videoQueue->getCurrentVideoId()) {
            openPlayer();
            return;
        }

        m_videoModel->fetch(m_videoQueue->getCurrentVideoId());
        m_sponsorBlockController->requestSponsors(m_videoQueue->getCurrentVideoId());

        openPlayer();
    });
    connect(this, &VideoController::playbackStateChanged, this, [this] {
        if (!m_currentPlayer) {
            return;
        }

        const bool paused = m_currentPlayer->paused();
        const bool durationMatchesPosition = abs(m_currentPlayer->duration() - m_currentPlayer->position()) < 0.1;
        const bool notAtBeginning = m_currentPlayer->position() > 1;

        if (paused && durationMatchesPosition && notAtBeginning) {
            m_videoQueue->next();
        }

        const bool shouldInhibit = currentPlayer() && !currentPlayer()->paused();

#ifdef HAS_DBUS
        if (shouldInhibit) {
            QDBusMessage message = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                                  QStringLiteral("/ScreenSaver"),
                                                                  QStringLiteral("org.freedesktop.ScreenSaver"),
                                                                  QStringLiteral("Inhibit"));
            message << QGuiApplication::desktopFileName();
            message << i18n("Playing video");

            QDBusReply<uint> reply = QDBusConnection::sessionBus().call(message);
            if (reply.isValid()) {
                screenSaverDbusCookie = reply.value();
            }
        } else {
            if (screenSaverDbusCookie != 0) {
                QDBusMessage message = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                                      QStringLiteral("/ScreenSaver"),
                                                                      QStringLiteral("org.freedesktop.ScreenSaver"),
                                                                      QStringLiteral("UnInhibit"));
                message << static_cast<uint>(screenSaverDbusCookie);
                screenSaverDbusCookie = 0;
                QDBusConnection::sessionBus().send(message);
            }
        }
#endif

#ifdef Q_OS_ANDROID
        QNativeInterface::QAndroidApplication::runOnAndroidMainThread([shouldInhibit] {
            QJniObject activity = QNativeInterface::QAndroidApplication::context();
            if (activity.isValid()) {
                QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

                if (window.isValid()) {
                    const int FLAG_KEEP_SCREEN_ON = 128;
                    if (shouldInhibit) {
                        window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
                    } else {
                        window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
                    }
                }
            }
        });
#endif
    });
    connect(this, &VideoController::positionChanged, this, [this] {
        auto currentPosition = position();
        if (m_sponsorBlockController->isSponsor(currentPosition)) {
            auto freePosition = m_sponsorBlockController->findFreePosition(currentPosition);
            if (freePosition.has_value()) {
                if (PlasmaTube::instance().settings()->skipSponsorBlock()) {
                    {
                        QSignalBlocker blocker(this); // ensure we don't enter a super loop
                        m_currentPlayer->setPosition(freePosition.value());
                    }
                    Q_EMIT skippedSponsorSegment();
                } else {
                    Q_EMIT enteredSponsorSegment(freePosition.value());
                }
            }
        }
    });
}

void VideoController::play(const QString &videoId)
{
    if (videoId.isEmpty()) {
        qCWarning(PLASMATUBE_GENERAL) << "Not trying to play an empty video id.";
        return;
    }

    m_videoQueue->replace({videoId});
}

void VideoController::queueNext(const QString &videoId)
{
    if (videoId.isEmpty()) {
        qCWarning(PLASMATUBE_GENERAL) << "Not trying to queue an empty video id.";
        return;
    }

    m_videoQueue->queueNext(videoId);
}

void VideoController::togglePlaying()
{
    if (m_currentPlayer != nullptr) {
        const bool paused = m_currentPlayer->paused();
        const bool durationMatchesPosition = abs(m_currentPlayer->duration() - m_currentPlayer->position()) < 0.1;
        const bool notAtBeginning = m_currentPlayer->position() > 1;

        if (paused && durationMatchesPosition && notAtBeginning) {
            m_currentPlayer->setPosition(0);
        }

        if (m_currentPlayer->paused()) {
            m_currentPlayer->play();
        } else {
            m_currentPlayer->pause();
        }
    }
}

void VideoController::stop()
{
    if (m_currentPlayer != nullptr) {
        Q_EMIT m_currentPlayer->command(QStringList() << QStringLiteral("stop"));
        videoModel()->clearVideo();
    }
}

void VideoController::previous()
{
    m_videoQueue->previous();
}

void VideoController::next()
{
    m_videoQueue->next();
}

VideoController::VideoMode VideoController::videoMode() const
{
    return m_videoMode;
}

void VideoController::setVideoMode(VideoController::VideoMode mode)
{
    if (mode != m_videoMode) {
        m_videoMode = mode;
        Q_EMIT videoModeChanged();

        openPlayer();
    }
}

MpvObject *VideoController::currentPlayer() const
{
    return m_currentPlayer;
}

void VideoController::setCurrentPlayer(MpvObject *mpvObject)
{
    std::optional<qreal> oldPosition;
    if (mpvObject != m_currentPlayer) {
        // stop existing player
        if (m_currentPlayer != nullptr) {
            Q_EMIT m_currentPlayer->command(QStringList() << QStringLiteral("stop"));
            oldPosition = m_currentPlayer->position();

            m_currentPlayer->disconnect(this);
        }

        m_currentPlayer = mpvObject;
        Q_EMIT currentPlayerChanged();
    }

    m_currentPlayer->setProperty(QStringLiteral("pause"), !PlasmaTube::instance().selectedSource()->preferences().autoPlay());

    m_currentPlayer->setAudioUrl(m_videoModel->audioUrl());
    Q_EMIT m_currentPlayer->command(QStringList() << QStringLiteral("stop"));
    // See https://github.com/mpv-player/mpv/issues/10029 why this is needed
    if (PlasmaTube::instance().selectedSource()->type() == VideoSource::Type::PeerTube) {
        m_currentPlayer->setProperty(QStringLiteral("stream-lavf-o"), QStringLiteral("seekable=0"));
    } else {
        m_currentPlayer->setProperty(QStringLiteral("stream-lavf-o"), QStringLiteral(""));
    }

    PlasmaTubeSettings settings(KSharedConfig::openConfig(QStringLiteral("plasmatuberc"), KConfig::SimpleConfig, QStandardPaths::AppConfigLocation));
    if (settings.proxyType() == 1) {
        QString proxyString = QStringLiteral("http://%1:%2@%3:%4")
                                  .arg(settings.proxyUser(), settings.proxyPassword(), settings.proxyHost(), QString::number(settings.proxyPort()));
        m_currentPlayer->setProperty(QStringLiteral("http-proxy"), proxyString);
        m_currentPlayer->setProperty(QStringLiteral("ytdl-raw-options"), proxyString);
    }

    Q_EMIT m_currentPlayer->command(QStringList() << QStringLiteral("loadfile") << m_videoModel->remoteUrl());
    m_currentPlayer->setProperty(QStringLiteral("ytdl-format"), QStringLiteral("best"));

    connect(m_currentPlayer, &MpvObject::positionChanged, this, &VideoController::positionChanged);
    connect(m_currentPlayer, &MpvObject::durationChanged, this, &VideoController::durationChanged);
    connect(m_currentPlayer, &MpvObject::pausedChanged, this, &VideoController::playbackStateChanged);
    connect(m_currentPlayer, &MpvObject::stoppedChanged, this, &VideoController::playbackStateChanged);

    // Restore old position if we had an existing player
    if (oldPosition != std::nullopt) {
        qDebug() << "Attempting to restore position to" << *oldPosition;
        connect(m_currentPlayer, &MpvObject::stoppedChanged, this, [this, oldPosition] {
            m_currentPlayer->seek(*oldPosition);
            qDebug() << "Set old position...";

            disconnect(m_currentPlayer);
        });
    }
}

VideoItem *VideoController::currentVideo() const
{
    return m_videoModel->video();
}

VideoModel *VideoController::videoModel() const
{
    return m_videoModel;
}

VideoQueue *VideoController::videoQueue() const
{
    return m_videoQueue;
}

void VideoController::openPlayer()
{
    if (m_videoMode == VideoMode::Normal) {
        Q_EMIT openNormalPlayer();
    } else {
        Q_EMIT openPiPPlayer();
    }
}

qreal VideoController::position() const
{
    return m_currentPlayer->position();
}

void VideoController::setPosition(qreal position)
{
    return m_currentPlayer->setPosition(position);
}

qreal VideoController::duration() const
{
    return m_currentPlayer->duration();
}

bool VideoController::paused() const
{
    return m_currentPlayer->paused();
}

void VideoController::pause()
{
    return m_currentPlayer->pause();
}

void VideoController::play()
{
    return m_currentPlayer->play();
}

bool VideoController::stopped() const
{
    return m_currentPlayer->stopped();
}

bool VideoController::hasVideo() const
{
    return m_currentPlayer != nullptr && currentVideo() != nullptr && currentVideo()->isLoaded();
}

#include "moc_videocontroller.cpp"