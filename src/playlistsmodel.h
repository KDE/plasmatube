// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QFutureWatcher>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

#include "comment.h"
#include "invidiousapi.h"

class PlaylistsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum CustomRoles { PlaylistIdRole = Qt::UserRole, TitleRole, ThumbnailRole, VideoCountRole };

    explicit PlaylistsModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void loadingChanged();

private:
    void fill();

    QFutureWatcher<QInvidious::PlaylistsResult> *m_futureWatcher = nullptr;
    QList<QInvidious::Playlist> m_playlists;
    bool m_loading = false;
};
