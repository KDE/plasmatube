/**
 * SPDX-FileCopyrightText: 2014 Sujith Haridasan <sujith.haridasan@kdemail.net>
 * SPDX-FileCopyrightText: 2014 Ashish Madeti <ashishmadeti@gmail.com>
 * SPDX-FileCopyrightText: 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * SPDX-FileCopyrightText: 2022-2023 Bart De Vries <bart@mogwai.be>
 * SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mediaplayer2player.h"
#include "mpris2logging.h"

#include "videocontroller.h"

#include <QCryptographicHash>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QMetaClassInfo>
#include <QStringList>
#include <QTimer>

MediaPlayer2Player::MediaPlayer2Player(VideoController *audioPlayer, QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_audioPlayer(audioPlayer)
    , mProgressIndicatorSignal(
          QDBusMessage::createSignal(QStringLiteral("/org/kde/kmediasession"), QStringLiteral("com.canonical.Unity.LauncherEntry"), QStringLiteral("Update")))
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::MediaPlayer2Player()";

    connect(m_audioPlayer, &VideoController::currentVideoChanged, this, &MediaPlayer2Player::setSource);
    connect(m_audioPlayer, &VideoController::currentVideoChanged, this, &MediaPlayer2Player::playerMetaDataChanged);
    connect(m_audioPlayer, &VideoController::playbackStateChanged, this, &MediaPlayer2Player::playerPlaybackStateChanged);
    connect(m_audioPlayer, &VideoController::durationChanged, this, &MediaPlayer2Player::audioDurationChanged);
    connect(m_audioPlayer, &VideoController::positionChanged, this, &MediaPlayer2Player::audioPositionChanged);
    connect(m_audioPlayer->videoQueue(), &VideoQueue::queueChanged, this, &MediaPlayer2Player::playerCanGoNextChanged);
    connect(m_audioPlayer->videoQueue(), &VideoQueue::queueChanged, this, &MediaPlayer2Player::playerCanGoPreviousChanged);

    if (m_audioPlayer->hasVideo()) {
        setSource();
    }
}

MediaPlayer2Player::~MediaPlayer2Player()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::~MediaPlayer2Player()";
}

QString MediaPlayer2Player::PlaybackStatus() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::PlaybackStatus()";
    QString result;

    if (!m_audioPlayer->hasVideo()) {
        return QStringLiteral("Stopped");
    }

    if (m_audioPlayer->stopped()) {
        result = QStringLiteral("Stopped");
    } else if (!m_audioPlayer->paused()) {
        result = QStringLiteral("Playing");
    } else {
        result = QStringLiteral("Paused");
    }

    return result;
}

bool MediaPlayer2Player::CanGoNext() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::CanGoNext()";
    if (m_audioPlayer) {
        return m_audioPlayer->videoQueue()->canGoNext();
    } else {
        return false;
    }
}

void MediaPlayer2Player::Next()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::Next()";
    m_audioPlayer->next();
}

bool MediaPlayer2Player::CanGoPrevious() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::CanGoPrevious()";
    if (m_audioPlayer) {
        return m_audioPlayer->videoQueue()->canGoPrevious();
    } else {
        return false;
    }
}

void MediaPlayer2Player::Previous()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::Previous()";
    m_audioPlayer->previous();
}

bool MediaPlayer2Player::CanPause() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::CanPause()";
    return true;
}

void MediaPlayer2Player::Pause()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::Pause()";
    if (m_audioPlayer)
        m_audioPlayer->pause();
}

void MediaPlayer2Player::PlayPause()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::PlayPause()";
    m_audioPlayer->togglePlaying();
}

void MediaPlayer2Player::Stop()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::Stop()";
    m_audioPlayer->stop();
}

bool MediaPlayer2Player::CanPlay() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::CanPlay()";
    return true;
}

void MediaPlayer2Player::Play()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::Play()";
    if (m_audioPlayer)
        m_audioPlayer->play();
}

double MediaPlayer2Player::Volume() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::Volume()";
    return m_volume;
}

void MediaPlayer2Player::setVolume(double volume)
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::setVolume(" << volume << ")";
    if (m_audioPlayer) {
        m_volume = qBound(0.0, volume, 1.0);
        Q_EMIT volumeChanged(m_volume);

        // TODO: app volume not implemented

        signalPropertiesChange(QStringLiteral("Volume"), Volume());
    }
}

QVariantMap MediaPlayer2Player::Metadata() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::Metadata()";
    return m_metadata;
}

qlonglong MediaPlayer2Player::Position() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::Position()";
    if (m_audioPlayer->hasVideo()) {
        return qlonglong(m_audioPlayer->position()) * 1000;
    } else {
        return 0.0;
    }
}

double MediaPlayer2Player::Rate() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::Rate()";
    return 1.0;
}

double MediaPlayer2Player::MinimumRate() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::MinimumRate()";
    return 1.0;
}

double MediaPlayer2Player::MaximumRate() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::MaximumRate()";
    return 1.0;
}

void MediaPlayer2Player::setRate(double newRate)
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::setRate(" << newRate << ")";
    if (newRate <= 0.0001 && newRate >= -0.0001) {
        Pause();
    }
}

bool MediaPlayer2Player::CanSeek() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::CanSeek()";
    return true;
}

bool MediaPlayer2Player::CanControl() const
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::CanControl()";
    return true;
}

void MediaPlayer2Player::Seek(qlonglong Offset)
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::Seek(" << Offset << ")";
    if (m_audioPlayer) {
        auto offset = (m_position + Offset) / 1000000;
        m_audioPlayer->setPosition(int(offset));
    }
}

void MediaPlayer2Player::SetPosition(const QDBusObjectPath &trackId, qlonglong pos)
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::SetPosition(" << pos << ")";
    if (m_audioPlayer) {
        if (m_audioPlayer->currentVideo() != nullptr) {
            if (trackId.path() == m_currentTrackId) {
                m_audioPlayer->setPosition(int(pos / 1000000));
            }
        }
    }
}

void MediaPlayer2Player::OpenUri(const QString &uri)
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::OpenUri(" << uri << ")";
    Q_UNUSED(uri);
}

void MediaPlayer2Player::playerPlaybackStateChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::playerPlaybackStateChanged()";
    signalPropertiesChange(QStringLiteral("PlaybackStatus"), PlaybackStatus());
    Q_EMIT playbackStatusChanged();
}

void MediaPlayer2Player::playerPlaybackRateChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::playerPlaybackRateChanged()";
    signalPropertiesChange(QStringLiteral("Rate"), Rate());
    Q_EMIT rateChanged(Rate());
}

void MediaPlayer2Player::playerSeeked(qint64 position)
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::playerSeeked(" << position << ")";
    Q_EMIT Seeked(position * 1000);
}

void MediaPlayer2Player::audioPositionChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::audioPositionChanged()";
    // for progress indicator on taskbar
    if (m_audioPlayer)
        setPropertyPosition(static_cast<int>(m_audioPlayer->position()));

    // Occasionally send updated position through MPRIS to make sure that
    // audio position is still correct if playing without seeking for a long
    // time.  This will also guarantee correct playback position if the MPRIS
    // client does not support non-standard playback rates
    qlonglong position = Position();
    if (abs(position - m_lastSentPosition) > 10000000) { // every 10 seconds
        m_lastSentPosition = position;
        Q_EMIT Seeked(position);
    }
}

void MediaPlayer2Player::audioDurationChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::audioDurationChanged()";
    // We reset all metadata in case the audioDuration changed
    // This is done because duration is not yet available when setEntry is
    // called (this is before the QMediaPlayer has read the new track
    if (m_audioPlayer) {
        qCDebug(Mpris2Log) << "Signal change of audio duration through MPRIS2" << m_audioPlayer->duration();
        if (m_audioPlayer->currentVideo() != nullptr) {
            m_metadata = getMetadataOfCurrentTrack();
            signalPropertiesChange(QStringLiteral("Metadata"), Metadata());
            signalPropertiesChange(QStringLiteral("CanPause"), CanPause());
            signalPropertiesChange(QStringLiteral("CanPlay"), CanPlay());
        }

        // for progress indicator on taskbar
        setPropertyPosition(static_cast<int>(m_audioPlayer->duration()));
    }
}

void MediaPlayer2Player::playerVolumeChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::playerVolumeChanged()";
    // TODO: app volume not implemented
}

void MediaPlayer2Player::playerCanPlayChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::playerCanPlayChanged()";
    signalPropertiesChange(QStringLiteral("CanPlay"), CanPlay());
}

void MediaPlayer2Player::playerCanPauseChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::playerCanPauseChanged()";
    signalPropertiesChange(QStringLiteral("CanPause"), CanPause());
}

void MediaPlayer2Player::playerCanGoNextChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::playerCanGoNextChanged()";
    signalPropertiesChange(QStringLiteral("CanGoNext"), CanGoNext());
}

void MediaPlayer2Player::playerCanGoPreviousChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::playerCanGoPreviousChanged()";
    signalPropertiesChange(QStringLiteral("CanGoPrevious"), CanGoNext());
}

void MediaPlayer2Player::playerCanSeekChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::playerCanSeekChanged()";
    signalPropertiesChange(QStringLiteral("CanSeek"), CanSeek());
}

void MediaPlayer2Player::playerMetaDataChanged()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::playerMetaDataChanged()";
    m_metadata = getMetadataOfCurrentTrack();
    signalPropertiesChange(QStringLiteral("Metadata"), Metadata());
}

void MediaPlayer2Player::setSource()
{
    auto video = m_audioPlayer->currentVideo();
    if (video == nullptr)
        return;

    qCDebug(Mpris2Log) << "MediaPlayer2Player::setSource(" << video->videoId() << ")";

    if (m_audioPlayer) {
        int queuenr = 0; // TODO: figure out smart way to handle this
        QString desktopName = QStringLiteral("/org.kde.plasmatube");
        desktopName.replace(QStringLiteral("."), QStringLiteral("/"));
        m_currentTrackId = QDBusObjectPath(desktopName + QLatin1String("/playlist/") + QString::number(queuenr)).path();

        m_metadata = getMetadataOfCurrentTrack();
        signalPropertiesChange(QStringLiteral("Metadata"), Metadata());
    }
}

QVariantMap MediaPlayer2Player::getMetadataOfCurrentTrack()
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::getMetadataOfCurrentTrack()";
    auto result = QVariantMap();

    if (m_currentTrackId.isEmpty()) {
        return {};
    }

    if (!m_audioPlayer->hasVideo()) {
        return {};
    }

    result[QStringLiteral("mpris:trackid")] = QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath(m_currentTrackId));
    result[QStringLiteral("mpris:length")] = qlonglong(m_audioPlayer->duration()) * 1000000; // TODO: what is this magic number?
    if (!m_audioPlayer->currentVideo()->title().isEmpty()) {
        result[QStringLiteral("xesam:title")] = m_audioPlayer->currentVideo()->title();
    }
    if (!m_audioPlayer->currentVideo()->author().isEmpty()) {
        result[QStringLiteral("xesam:artist")] = m_audioPlayer->currentVideo()->author();
        result[QStringLiteral("xesam:album")] = m_audioPlayer->currentVideo()->author();
    }
    if (!m_audioPlayer->currentVideo()->thumbnailUrl(QStringLiteral("medium")).isEmpty()) {
        result[QStringLiteral("mpris:artUrl")] = m_audioPlayer->currentVideo()->thumbnailUrl(QStringLiteral("medium")).toString();
    }

    return result;
}

void MediaPlayer2Player::setPropertyPosition(int newPositionInMs)
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::setPropertyPosition(" << newPositionInMs << ")";
    // only needed for progressbar on taskbar (?)
    m_position = qlonglong(newPositionInMs) * 1000;
}

void MediaPlayer2Player::signalPropertiesChange(const QString &property, const QVariant &value)
{
    qCDebug(Mpris2Log) << "MediaPlayer2Player::signalPropertiesChange(" << property << value << ")";
    // qDebug() << "mpris signal property change" << property << value;
    QVariantMap properties;
    properties[property] = value;
    const int ifaceIndex = metaObject()->indexOfClassInfo("D-Bus Interface");
    QDBusMessage msg = QDBusMessage::createSignal(QStringLiteral("/org/mpris/MediaPlayer2"),
                                                  QStringLiteral("org.freedesktop.DBus.Properties"),
                                                  QStringLiteral("PropertiesChanged"));

    msg << QLatin1String(metaObject()->classInfo(ifaceIndex).value());
    msg << properties;
    msg << QStringList();

    QDBusConnection::sessionBus().send(msg);
}
