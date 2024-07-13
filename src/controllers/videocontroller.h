// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "mpris2.h"
#include "mpvobject.h"
#include "sponsorblockcontroller.h"
#include "videomodel.h"
#include "videoqueue.h"

#include <QObject>
#include <QQmlEngine>
#include <QtQml>

class VideoController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Access via PlasmaTube.VideoController")

    Q_PROPERTY(VideoMode videoMode READ videoMode WRITE setVideoMode NOTIFY videoModeChanged)
    Q_PROPERTY(MpvObject *currentPlayer READ currentPlayer WRITE setCurrentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(VideoItem *currentVideo READ currentVideo NOTIFY currentVideoChanged)
    Q_PROPERTY(VideoModel *videoModel READ videoModel CONSTANT)
    Q_PROPERTY(VideoQueue *videoQueue READ videoQueue CONSTANT)

public:
    explicit VideoController(QObject *parent = nullptr);

    Q_INVOKABLE void play(const QString &videoId);
    Q_INVOKABLE void queueNext(const QString &videoId);

    Q_INVOKABLE void togglePlaying();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void next();

    enum VideoMode { Normal, PictureInPicture };
    Q_ENUM(VideoMode);

    VideoMode videoMode() const;
    void setVideoMode(VideoMode mode);

    MpvObject *currentPlayer() const;
    void setCurrentPlayer(MpvObject *mpvObject);

    VideoItem *currentVideo() const;

    VideoModel *videoModel() const;
    VideoQueue *videoQueue() const;

    qreal position() const;
    void setPosition(qreal position);

    qreal duration() const;

    bool paused() const;
    void pause();
    void play();

    bool stopped() const;
    bool hasVideo() const;

Q_SIGNALS:
    void videoModeChanged();
    void currentPlayerChanged();
    void currentVideoChanged();
    void openNormalPlayer();
    void openPiPPlayer();

    void positionChanged();
    void durationChanged();
    void playbackStateChanged();

private:
    void openPlayer();

    VideoMode m_videoMode = VideoMode::Normal;
    MpvObject *m_currentPlayer = nullptr;
    VideoModel *m_videoModel = nullptr;
    VideoQueue *m_videoQueue = nullptr;
    Mpris2 *m_mpris = nullptr;
    unsigned int screenSaverDbusCookie = 0;
    SponsorBlockController *m_sponsorBlockController = nullptr;
};
