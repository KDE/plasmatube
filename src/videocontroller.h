// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>

#include "mpvobject.h"
#include "videomodel.h"

class VideoController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Access via PlasmaTube.VideoController")

    Q_PROPERTY(VideoMode videoMode READ videoMode WRITE setVideoMode NOTIFY videoModeChanged)
    Q_PROPERTY(MpvObject *currentPlayer READ currentPlayer WRITE setCurrentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(VideoItem *currentVideo READ currentVideo NOTIFY currentVideoChanged)
    Q_PROPERTY(VideoModel *videoModel READ videoModel CONSTANT)

public:
    explicit VideoController(QObject *parent = nullptr);

    Q_INVOKABLE void play(const QString &videoId);

    enum VideoMode { Normal, PictureInPicture };
    Q_ENUM(VideoMode);

    VideoMode videoMode() const;
    void setVideoMode(VideoMode mode);

    MpvObject *currentPlayer() const;
    void setCurrentPlayer(MpvObject *mpvObject);

    VideoItem *currentVideo() const;

    VideoModel *videoModel() const;

Q_SIGNALS:
    void videoModeChanged();
    void currentPlayerChanged();
    void currentVideoChanged();
    void openNormalPlayer();
    void openPiPPlayer();

private:
    void openPlayer();

    VideoMode m_videoMode = VideoMode::Normal;
    MpvObject *m_currentPlayer = nullptr;
    VideoModel *m_videoModel = nullptr;
};
