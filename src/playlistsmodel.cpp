// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "playlistsmodel.h"
#include "plasmatube.h"

using namespace Qt::Literals::StringLiterals;

PlaylistsModel::PlaylistsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fill();
}

QVariant PlaylistsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &comment = m_playlists[index.row()];

    switch (role) {
    case TitleRole:
        return comment.title();
    case ThumbnailRole:
        return comment.thumbnail();
    case VideoCountRole:
        return comment.videoCount();
    default:
        return {};
    }
}

bool PlaylistsModel::loading() const
{
    return m_loading;
}

void PlaylistsModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int PlaylistsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_playlists.size();
}

QHash<int, QByteArray> PlaylistsModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {ThumbnailRole, "thumbnail"},
        {VideoCountRole, "videoCount"},
    };
}

void PlaylistsModel::fill()
{
    if (m_loading) {
        return;
    }
    setLoading(true);

    m_futureWatcher = new QFutureWatcher<QInvidious::PlaylistsResult>();

    auto future = PlasmaTube::instance().api()->requestPlaylists();
    m_futureWatcher->setFuture(future);

    connect(m_futureWatcher, &QFutureWatcherBase::finished, this, [this] {
        auto result = m_futureWatcher->result();
        if (auto playlists = std::get_if<QList<QInvidious::Playlist>>(&result)) {
            const auto rows = rowCount({});
            beginInsertRows({}, rows, rows + playlists->size() - 1);
            m_playlists << *playlists;
            endInsertRows();
        } else if (auto error = std::get_if<QInvidious::Error>(&result)) {
            // TODO: Log error
        }

        m_futureWatcher->deleteLater();
        m_futureWatcher = nullptr;
        setLoading(false);
    });
}
