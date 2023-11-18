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

private:
    void fill();

    QFutureWatcher<QInvidious::PlaylistsResult> *m_futureWatcher = nullptr;
    QList<QInvidious::Playlist> m_playlists;
};
