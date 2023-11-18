// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videolistmodel.h"

#include "plasmatube.h"

#include <KLocalizedString>

#include <QFutureWatcher>
#include <QNetworkReply>
#include <QtConcurrent>

QString VideoListModel::queryTypeString(QueryType type)
{
    switch (type) {
    case Feed:
        return i18n("Subscriptions");
    case Top:
        return i18n("Popular");
    case Trending:
        return i18nc("@action:button All trending videos", "All");
    case TrendingGaming:
        return i18nc("@action:button Trending gaming videos", "Gaming");
    case TrendingMovies:
        return i18nc("@action:button Trending movie videos", "Movies");
    case TrendingMusic:
        return i18nc("@action:button Trending music videos", "Music");
    case TrendingNews:
        return i18nc("@action:button Trending news videos", "News");
    case History:
        return i18nc("@action:button Video watch history", "History");
    default:
        return {};
    }
}

QString VideoListModel::queryTypeIcon(QueryType type)
{
    switch (type) {
    case Feed:
        return QStringLiteral("feed-subscribe");
    case Top:
        return QStringLiteral("arrow-up-double");
    case Trending:
        // should actually be "user-trash-full-symbolic"
        return QStringLiteral("favorite");
    case TrendingGaming:
        return QStringLiteral("folder-games-symbolic");
    case TrendingMovies:
        return QStringLiteral("folder-videos-symbolic");
    case TrendingMusic:
        return QStringLiteral("folder-music-symbolic");
    case TrendingNews:
        return QStringLiteral("news-subscribe");
    case History:
        return QStringLiteral("view-history");
    default:
        return {};
    }
}

VideoListModel::VideoListModel(QObject *parent)
    : AbstractListModel(parent)
{
}

VideoListModel::VideoListModel(const QList<QInvidious::VideoBasicInfo> &list, QObject *parent)
    : AbstractListModel(parent)
    , m_results(list)
{
}

int VideoListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_results.size());
}

QVariant VideoListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.parent().isValid())
        return {};

    const QInvidious::VideoBasicInfo &video = m_results.at(index.row());
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
    default:
        break;
    }
    return {};
}

void VideoListModel::fetchMore(const QModelIndex &index)
{
    if (canFetchMore(index)) {
        switch (m_queryType) {
        case Feed: {
            auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestFeed(++m_currentPage);
            handleQuery(future, Feed, false);
            break;
        }
        case Channel: {
            auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestChannel(m_channel, ++m_currentPage);
            handleQuery(future, Channel, false);
            break;
        }
        case History: {
            m_currentPage++;
            requestQuery(History);
        } break;
        default: {
        }
        }
    }
}

bool VideoListModel::canFetchMore(const QModelIndex &) const
{
    return !m_historyPageWatcher && !m_futureWatcher && (m_queryType == Search || m_queryType == Channel || m_queryType == Feed || m_queryType == History);
}

QString VideoListModel::title() const
{
    switch (m_queryType) {
    case Search:
        return i18n("Search results for \"%1\"").arg(m_searchParameters.query());
    default:
        return queryTypeString(m_queryType);
    }
}

void VideoListModel::requestChannel(const QString &ucid)
{
    m_channel = ucid;
    m_currentPage = 1;
    handleQuery(PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestChannel(ucid, m_currentPage), Channel);
}

void VideoListModel::requestPlaylist(const QString &id)
{
    m_playlist = id;
    m_currentPage = 1;
    handleQuery(PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestPlaylist(id), Playlist);
}

void VideoListModel::requestQuery(QueryType type)
{
    auto selectedSource = PlasmaTube::instance().selectedSource();
    if (selectedSource == nullptr) {
        return;
    }

    m_searchParameters.clear();
    switch (type) {
    case Feed:
        handleQuery(selectedSource->api()->requestFeed(), type);
        break;
    case Top:
        handleQuery(selectedSource->api()->requestTop(), type);
        break;
    case Trending:
        handleQuery(selectedSource->api()->requestTrending(), type);
        break;
    case TrendingGaming:
        handleQuery(selectedSource->api()->requestTrending(QInvidious::Gaming), type);
        break;
    case TrendingMovies:
        handleQuery(selectedSource->api()->requestTrending(QInvidious::Movies), type);
        break;
    case TrendingMusic:
        handleQuery(selectedSource->api()->requestTrending(QInvidious::Music), type);
        break;
    case TrendingNews:
        handleQuery(selectedSource->api()->requestTrending(QInvidious::News), type);
        break;
    case History:
        requestHistory();
        break;
    default:
        qDebug() << "VideoListModel::requestQuery() called with not allowed type" << type;
    }
}

void VideoListModel::refresh()
{
    if (!isLoading() && m_queryType != NoQuery) {
        switch (m_queryType) {
        case Channel:
            requestChannel(m_channel);
            break;
        default:
            requestQuery(m_queryType);
        }
    }
}

void VideoListModel::handleQuery(QFuture<QInvidious::VideoListResult> future, QueryType type, bool reset)
{
    // reset loaded videos (if requested)
    if (reset) {
        clearAll();
    }

    // stop running task
    if (m_futureWatcher) {
        // TODO: cancelling isn't implemented yet
        m_futureWatcher->cancel();
        m_futureWatcher->deleteLater();
        m_futureWatcher = nullptr;
    }

    // set up new task
    m_futureWatcher = new QFutureWatcher<QInvidious::VideoListResult>();
    connect(m_futureWatcher, &QFutureWatcherBase::finished, this, [this] {
        auto result = m_futureWatcher->result();
        if (auto videos = std::get_if<QList<QInvidious::VideoBasicInfo>>(&result)) {
            const auto rows = rowCount();
            beginInsertRows({}, rows, rows + videos->size() - 1);
            m_results << *videos;
            endInsertRows();
        } else if (auto error = std::get_if<QInvidious::Error>(&result)) {
            Q_EMIT errorOccured(error->second);
        }

        m_futureWatcher->deleteLater();
        m_futureWatcher = nullptr;
        Q_EMIT isLoadingChanged();
    });

    setQueryType(type);
    m_futureWatcher->setFuture(future);
    Q_EMIT isLoadingChanged();
}

void VideoListModel::setQueryType(QueryType type)
{
    // title changes if the type changes or the search string has been updated (page 0)
    if (m_queryType != type || m_searchParameters.query().isEmpty()) {
        m_queryType = type;
        Q_EMIT titleChanged();
    }
}

void VideoListModel::clearAll()
{
    beginResetModel();
    m_results.clear();
    endResetModel();
}

void VideoListModel::requestHistory()
{
    auto pageFuture = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestHistory(m_currentPage);

    m_historyPageWatcher = new QFutureWatcher<QInvidious::HistoryResult>();
    m_historyPageWatcher->setFuture(pageFuture);

    connect(m_historyPageWatcher, &QFutureWatcherBase::finished, this, [this] {
        auto result = m_historyPageWatcher->future().result();
        if (auto historyList = std::get_if<QList<QString>>(&result)) {
            processHistoryResult(*historyList);
        } else if (auto error = std::get_if<QInvidious::Error>(&result)) {
            Q_EMIT errorOccured(error->second);
        }

        m_historyPageWatcher->deleteLater();
        m_historyPageWatcher = nullptr;
    });

    setQueryType(History);
    Q_EMIT isLoadingChanged();
}

void VideoListModel::processHistoryResult(const QList<QString> &result)
{
    for (const auto &videoId : result) {
        auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestVideo(videoId);
        m_historyFutureSync.addFuture(future);
    }

    m_historyFetchFinishWatcher = new QFutureWatcher<void>();
    m_historyFetchFinishWatcher->setFuture(QtConcurrent::run([this] {
        m_historyFutureSync.waitForFinished();
    }));

    connect(m_historyFetchFinishWatcher, &QFutureWatcherBase::finished, this, [this] {
        for (const auto &future : m_historyFutureSync.futures()) {
            auto result = future.result();
            if (auto video = std::get_if<QInvidious::Video>(&result)) {
                const auto rows = rowCount();
                beginInsertRows({}, rows, rows);
                m_results.push_back(QInvidious::VideoBasicInfo(*video));
                endInsertRows();
            } else if (auto error = std::get_if<QInvidious::Error>(&result)) {
                Q_EMIT errorOccured(error->second);
            }
        }

        m_historyFutureSync.clearFutures();

        m_historyFetchFinishWatcher->deleteLater();
        m_historyFetchFinishWatcher = nullptr;
        Q_EMIT isLoadingChanged();
    });
}

void VideoListModel::markAsWatched(int row)
{
    auto videoIndex = index(row, 0);
    Q_ASSERT(checkIndex(videoIndex, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    PlasmaTube::instance().selectedSource()->markVideoWatched(data(videoIndex, IdRole).toString());
    Q_EMIT dataChanged(videoIndex, videoIndex, {WatchedRole});
}

void VideoListModel::markAsUnwatched(int row)
{
    auto videoIndex = index(row, 0);
    Q_ASSERT(checkIndex(videoIndex, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    PlasmaTube::instance().selectedSource()->markVideoUnwatched(data(videoIndex, IdRole).toString());
    Q_EMIT dataChanged(videoIndex, videoIndex, {WatchedRole});
}

void VideoListModel::removeFromPlaylist(const QString &plid, int index)
{
    auto video = m_results[index];

    PlasmaTube::instance().sourceManager()->selectedSource()->api()->removeVideoFromPlaylist(plid, video.indexId());

    beginRemoveRows({}, index, index);
    m_results.removeAt(index);
    endRemoveRows();
}

#include "moc_videolistmodel.cpp"