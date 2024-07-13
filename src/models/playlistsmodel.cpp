// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "playlistsmodel.h"

#include "plasmatube.h"

using namespace Qt::Literals::StringLiterals;

PlaylistsModel::PlaylistsModel(QObject *parent)
    : AbstractListModel(parent)
{
}

QVariant PlaylistsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &comment = m_playlists[index.row()];

    switch (role) {
    case IdRole:
        return comment.id();
    case TypeRole:
        return QStringLiteral("playlist");
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

int PlaylistsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_playlists.size();
}

void PlaylistsModel::fill(const QFuture<QInvidious::PlaylistsResult> &future)
{
    if (isLoading()) {
        return;
    }
    setLoading(true);

    m_futureWatcher = new QFutureWatcher<QInvidious::PlaylistsResult>();
    m_futureWatcher->setFuture(future);

    connect(m_futureWatcher, &QFutureWatcherBase::finished, this, [this] {
        auto result = m_futureWatcher->result();
        if (auto playlists = std::get_if<QList<QInvidious::Playlist>>(&result)) {
            const auto rows = rowCount({});
            beginInsertRows({}, rows, rows + playlists->size() - 1);
            m_playlists << *playlists;
            endInsertRows();
        } else if (auto error = std::get_if<QInvidious::Error>(&result)) {
            qWarning() << "Failed to fetch playlists:" << error->second;
        }

        m_futureWatcher->deleteLater();
        m_futureWatcher = nullptr;
        setLoading(false);
    });
}

void PlaylistsModel::loadUserPlaylists()
{
    fill(PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestPlaylists());
}

void PlaylistsModel::loadChannelPlaylists(const QString &channelId)
{
    fill(PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestChannelPlaylists(channelId));
}

void PlaylistsModel::refresh()
{
    // TODO: implement
}

void PlaylistsModel::markAsWatched(int index)
{
    // intentional, this function is only relevant for videos
    Q_UNUSED(index)
}

void PlaylistsModel::markAsUnwatched(int index)
{
    // intentional, this function is only relevant for videos
    Q_UNUSED(index)
}

void PlaylistsModel::removeFromPlaylist(const QString &plid, int index)
{
    // intentional, this function is only relevant for videos
    Q_UNUSED(plid)
    Q_UNUSED(index)
}

#include "moc_playlistsmodel.cpp"