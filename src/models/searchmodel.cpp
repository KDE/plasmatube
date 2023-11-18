// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "searchmodel.h"
#include "controllers/plasmatube.h"

#include <QFutureWatcher>
#include <QNetworkReply>
#include <QtConcurrent>

#include <KLocalizedString>

SearchModel::SearchModel(QObject *parent)
    : AbstractListModel(parent)
{
}

int SearchModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_results.size();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.parent().isValid())
        return {};

    const QInvidious::SearchResult &result = m_results.at(index.row());
    if (result.type() == QInvidious::SearchResult::Type::Video) {
        auto &video = result.video();
        switch (role) {
        case IdRole:
            return video.videoId();
        case TypeRole:
            return QStringLiteral("video");
        case TitleRole:
            return video.title();
        case ThumbnailRole: {
            const auto thumbnailUrl = video.thumbnail(QStringLiteral("medium")).url();
            if (thumbnailUrl.isRelative()) {
                return QUrl(PlasmaTube::instance().sourceManager()->selectedSource()->api()->apiHost() + thumbnailUrl.toString(QUrl::FullyEncoded));
            }
            return thumbnailUrl;
        }
        case LengthRole:
            return video.length();
        case ViewCountRole:
            return video.viewCount();
        case AuthorRole:
            return video.author();
        case AuthorIdRole:
            return video.authorId();
        case AuthorUrlRole:
            return video.authorUrl();
        case PublishedRole:
            return video.published();
        case PublishedTextRole:
            return video.publishedText();
        case DescriptionRole:
            return video.description();
        case DescriptionHtmlRole:
            return video.descriptionHtml();
        case LiveNowRole:
            return video.liveNow();
        case PaidRole:
            return video.paid();
        case PremiumRole:
            return video.premium();
        case WatchedRole:
            return PlasmaTube::instance().selectedSource()->isVideoWatched(video.videoId());
        }
    } else if (result.type() == QInvidious::SearchResult::Type::Channel) {
        auto &channel = result.channel();
        switch (role) {
        case IdRole:
            return channel.id();
        case TypeRole:
            return QStringLiteral("channel");
        case ChannelNameRole:
            return channel.name();
        case ChannelAvatarRole:
            return channel.avatar();
        }
    } else if (result.type() == QInvidious::SearchResult::Type::Playlist) {
        auto &playlist = result.playlist();
        switch (role) {
        case IdRole:
            return playlist.id();
        case TypeRole:
            return QStringLiteral("playlist");
        case TitleRole:
            return playlist.title();
        case VideoCountRole:
            return playlist.videoCount();
        case ThumbnailRole:
            return playlist.thumbnail();
        }
    }

    return {};
}

void SearchModel::fetchMore(const QModelIndex &index)
{
    if (canFetchMore(index)) {
        m_currentPage++;
        performSearch();
    }
}

bool SearchModel::canFetchMore(const QModelIndex &) const
{
    return !m_futureWatcher;
}

bool SearchModel::isLoading() const
{
    return m_futureWatcher != nullptr;
}

void SearchModel::request(const SearchParameters *searchParameters)
{
    if (!m_results.isEmpty()) {
        beginResetModel();
        m_results.clear();
        endResetModel();
    }

    m_currentPage = 1;
    m_searchParameters.fill(*searchParameters);
    performSearch();
}

void SearchModel::refresh()
{
}

void SearchModel::markAsWatched(int index)
{
}

void SearchModel::markAsUnwatched(int index)
{
}

void SearchModel::removeFromPlaylist(const QString &plid, int index)
{
}

void SearchModel::performSearch()
{
    auto selectedSource = PlasmaTube::instance().selectedSource();
    if (selectedSource == nullptr) {
        return;
    }

    m_searchParameters.setPage(m_currentPage);

    // stop running task
    if (m_futureWatcher) {
        // TODO: cancelling isn't implemented yet
        m_futureWatcher->cancel();
        m_futureWatcher->deleteLater();
        m_futureWatcher = nullptr;
    }

    // set up new task
    m_futureWatcher = new QFutureWatcher<QInvidious::SearchListResult>();
    connect(m_futureWatcher, &QFutureWatcherBase::finished, this, [this] {
        auto result = m_futureWatcher->result();
        if (auto videos = std::get_if<QList<QInvidious::SearchResult>>(&result)) {
            const auto rows = rowCount();
            beginInsertRows({}, rows, rows + videos->size() - 1);
            m_results << *videos;
            endInsertRows();
        } else if (auto error = std::get_if<QInvidious::Error>(&result)) {
            Q_EMIT errorOccured(error->second);
        }

        m_futureWatcher->deleteLater();
        m_futureWatcher = nullptr;
        setLoading(false);
    });

    m_futureWatcher->setFuture(selectedSource->api()->requestSearchResults(m_searchParameters));
    setLoading(true);
}

#include "moc_searchmodel.cpp"