// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videocontroller.h"

#include "config.h"
#include "plasmatube.h"

VideoController::VideoController(QObject *parent)
    : QObject(parent)
    , m_videoModel(new VideoModel(this))
{
    connect(m_videoModel, &VideoModel::videoChanged, this, [this] {
        Q_EMIT currentVideoChanged();
    });
}

void VideoController::play(const QString &videoId)
{
    // If the user is attempting to "play" the same video, they probably meant to open the player
    if (currentVideo() && currentVideo()->videoId() == videoId) {
        openPlayer();
        return;
    }

    m_videoModel->fetch(videoId);

    openPlayer();
}

void VideoController::togglePlaying()
{
    if (m_currentPlayer != nullptr) {
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
        m_currentPlayer->command(QStringList() << QStringLiteral("stop"));
        videoModel()->clearVideo();
    }
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
            m_currentPlayer->command(QStringList() << QStringLiteral("stop"));
            oldPosition = m_currentPlayer->position();
        }

        m_currentPlayer = mpvObject;
        Q_EMIT currentPlayerChanged();
    }

    m_currentPlayer->command(QStringList() << QStringLiteral("stop"));
    // See https://github.com/mpv-player/mpv/issues/10029 why this is needed
    if (PlasmaTube::instance().selectedSource()->type() == VideoSource::Type::PeerTube) {
        m_currentPlayer->setOption(QStringLiteral("stream-lavf-o"), QStringLiteral("seekable=0"));
    } else {
        m_currentPlayer->setOption(QStringLiteral("stream-lavf-o"), QStringLiteral(""));
    }

    PlasmaTubeSettings settings(KSharedConfig::openConfig(QStringLiteral("plasmatuberc"), KConfig::SimpleConfig, QStandardPaths::AppConfigLocation));
    if (settings.proxyType() == 1) {
        QString proxyString = QStringLiteral("http://%1:%2@%3:%4")
                                  .arg(settings.proxyUser(), settings.proxyPassword(), settings.proxyHost(), QString::number(settings.proxyPort()));
        m_currentPlayer->setOption(QStringLiteral("http-proxy"), proxyString);
        m_currentPlayer->setOption(QStringLiteral("ytdl-raw-options"), proxyString);
    }

    m_currentPlayer->command(QStringList() << QStringLiteral("loadfile")
                                           << PlasmaTube::instance().selectedSource()->api()->resolveVideoUrl(m_videoModel->videoId()));
    m_currentPlayer->setOption(QStringLiteral("ytdl-format"), QStringLiteral("best"));

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

void VideoController::openPlayer()
{
    if (m_videoMode == VideoMode::Normal) {
        Q_EMIT openNormalPlayer();
    } else {
        Q_EMIT openPiPPlayer();
    }
}
