// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "videobasicinfo.h"

#include <QAbstractListModel>
#include <QFutureWatcher>
#include <QtQml>

#include <abstractapi.h>

class VideoQueue : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Use PlasmaTube.videoController")

    Q_PROPERTY(bool shouldBeVisible READ shouldBeVisible NOTIFY queueChanged)

public:
    explicit VideoQueue(QObject *parent = nullptr);

    void replace(const QStringList &videoIds);
    void queueNext(const QString &videoId);
    Q_INVOKABLE void clear();

    /**
     * @brief Plays the @p index video in the current queue.
     */
    Q_INVOKABLE void playInQueue(int index);

    /**
     * @brief Replaces the queue with the videos from @p playlistId
     */
    Q_INVOKABLE void loadPlaylist(const QString &playlistId);

    /**
     * @brief Replaces the queue with the videos from @p playlistId and then plays @p videoId
     */
    Q_INVOKABLE void loadPlaylistAndPlay(const QString &playlistId, const QString &videoId);

    bool shouldBeVisible() const;

    void next();
    bool canGoNext() const;

    void previous();
    bool canGoPrevious() const;

    QString getCurrentVideoId() const;

    // TODO: we should have an AbstractVideoListModel, and combine these roles with VideoLisTModel
    enum Roles : int {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ThumbnailRole,
        LengthRole,
        ViewCountRole,
        AuthorRole,
        AuthorIdRole,
        AuthorUrlRole,
        PublishedRole,
        PublishedTextRole,
        DescriptionRole,
        DescriptionHtmlRole,
        LiveNowRole,
        PaidRole,
        PremiumRole,
        WatchedRole,
        PlayingRole
    };

    [[nodiscard]] int rowCount(const QModelIndex &index) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void currentVideoChanged();
    void queueChanged();

private:
    void setCurrentIndex(int newIndex);
    void requestMissingVideoInformation();

    QList<QString> m_videoIds;
    QList<std::optional<QInvidious::VideoBasicInfo>> m_videoInfo;
    int m_currentIndex = 0;
};
