// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractlistmodel.h"
#include "comment.h"

#include <abstractapi.h>

#include <QAbstractListModel>
#include <QDateTime>
#include <QFutureWatcher>
#include <QUrl>
#include <QtQml>

class PlaylistsModel : public AbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit PlaylistsModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;

    Q_INVOKABLE void loadUserPlaylists();
    Q_INVOKABLE void loadChannelPlaylists(const QString &channelId);

public Q_SLOTS:
    void refresh() override;
    void markAsWatched(int index) override;
    void markAsUnwatched(int index) override;
    void removeFromPlaylist(const QString &plid, int index) override;

private:
    void fill(const QFuture<QInvidious::PlaylistsResult> &future);

    QFutureWatcher<QInvidious::PlaylistsResult> *m_futureWatcher = nullptr;
    QList<QInvidious::Playlist> m_playlists;
};
