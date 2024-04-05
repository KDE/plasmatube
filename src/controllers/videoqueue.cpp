// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videoqueue.h"

#include "plasmatube.h"

VideoQueue::VideoQueue(QObject *parent)
    : QAbstractListModel(parent)
{
}

void VideoQueue::replace(const QStringList &videoIds)
{
    beginResetModel();
    m_videoIds = videoIds;
    m_videoInfo.clear();
    m_videoInfo.resize(videoIds.size());
    endResetModel();
    Q_EMIT queueChanged();

    requestMissingVideoInformation();

    setCurrentIndex(0);
}

void VideoQueue::queueNext(const QString &videoId)
{
    beginInsertRows({}, m_videoIds.size(), m_videoIds.size());
    m_videoIds.push_back(videoId);
    m_videoInfo.push_back(std::nullopt);
    endInsertRows();
    Q_EMIT queueChanged();

    requestMissingVideoInformation();
}

void VideoQueue::clear()
{
    beginResetModel();
    m_videoIds.clear();
    m_videoInfo.clear();
    endResetModel();
    Q_EMIT queueChanged();

    setCurrentIndex(0);
}

void VideoQueue::playInQueue(const int videoIndex)
{
    // TODO: protect against invalid indexes
    setCurrentIndex(videoIndex);
}

void VideoQueue::loadPlaylist(const QString &playlistId)
{
    auto playlistFuture = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestPlaylist(playlistId);

    auto playlistFutureWatcher = new QFutureWatcher<QInvidious::VideoListResult>(this);
    connect(playlistFutureWatcher, &QFutureWatcherBase::finished, this, [this, playlistFutureWatcher] {
        auto result = playlistFutureWatcher->result();

        if (const auto videoList = std::get_if<QList<QInvidious::VideoBasicInfo>>(&result)) {
            QStringList videoIdList;
            std::transform(videoList->cbegin(), videoList->cend(), std::back_inserter(videoIdList), [](const auto &video) {
                return video.videoId();
            });

            replace(videoIdList);
        } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
            qDebug() << "VideoQueue::loadPlaylist(): Error:" << error->second << error->first;
        }

        playlistFutureWatcher->deleteLater();
    });
    playlistFutureWatcher->setFuture(playlistFuture);
}

void VideoQueue::loadPlaylistAndPlay(const QString &playlistId, const QString &videoId)
{
    // TODO: merge with loadPlaylist
    auto playlistFuture = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestPlaylist(playlistId);

    auto playlistFutureWatcher = new QFutureWatcher<QInvidious::VideoListResult>(this);
    connect(playlistFutureWatcher, &QFutureWatcherBase::finished, this, [this, videoId, playlistFutureWatcher] {
        auto result = playlistFutureWatcher->result();

        if (const auto videoList = std::get_if<QList<QInvidious::VideoBasicInfo>>(&result)) {
            QStringList videoIdList;
            std::transform(videoList->cbegin(), videoList->cend(), std::back_inserter(videoIdList), [](const auto &video) {
                return video.videoId();
            });

            replace(videoIdList);

            qsizetype index = videoIdList.indexOf(videoId);
            playInQueue(index);
        } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
            qDebug() << "VideoQueue::loadPlaylist(): Error:" << error->second << error->first;
        }

        playlistFutureWatcher->deleteLater();
    });
    playlistFutureWatcher->setFuture(playlistFuture);
}

void VideoQueue::next()
{
    if (canGoNext()) {
        setCurrentIndex(m_currentIndex + 1);
    }
}

bool VideoQueue::canGoNext() const
{
    return m_currentIndex + 1 < m_videoIds.size();
}

void VideoQueue::previous()
{
    if (canGoPrevious()) {
        m_currentIndex--;
        Q_EMIT currentVideoChanged();
    }
}

bool VideoQueue::canGoPrevious() const
{
    return (m_currentIndex - 1) >= 0;
}

QString VideoQueue::getCurrentVideoId() const
{
    if (m_videoIds.isEmpty()) {
        return {};
    }
    return m_videoIds[m_currentIndex];
}

int VideoQueue::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return static_cast<int>(m_videoIds.size());
}

QVariant VideoQueue::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index)) {
        return {};
    }

    const int row = index.row();
    if (role == IdRole) {
        return m_videoIds[row];
    } else if (role == PlayingRole) {
        return row == m_currentIndex;
    }

    if (m_videoInfo[row] != std::nullopt) {
        const auto &video = *m_videoInfo[row];
        switch (role) {
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
    }

    return {};
}

QHash<int, QByteArray> VideoQueue::roleNames() const
{
    return {{IdRole, "id"},
            {TitleRole, "title"},
            {ThumbnailRole, "thumbnail"},
            {LengthRole, "length"},
            {ViewCountRole, "viewCount"},
            {AuthorRole, "author"},
            {AuthorIdRole, "authorId"},
            {AuthorUrlRole, "authorUrl"},
            {PublishedRole, "published"},
            {PublishedTextRole, "publishedText"},
            {DescriptionRole, "description"},
            {DescriptionHtmlRole, "descriptionHtml"},
            {LiveNowRole, "liveNow"},
            {PaidRole, "paid"},
            {PremiumRole, "premium"},
            {WatchedRole, "watched"},
            {PlayingRole, "playing"}};
}

void VideoQueue::setCurrentIndex(const int newIndex)
{
    const int oldIndex = m_currentIndex;
    m_currentIndex = newIndex;
    Q_EMIT currentVideoChanged();

    Q_EMIT dataChanged(index(oldIndex, 0), index(oldIndex, 0), {PlayingRole});

    Q_EMIT dataChanged(index(m_currentIndex, 0), index(m_currentIndex, 0), {PlayingRole});
}

void VideoQueue::requestMissingVideoInformation()
{
    size_t i = 0;
    for (const auto &video : m_videoInfo) {
        // TODO: this doesn't cover the case where we could add another video while it's still fetchibng
        if (video == std::nullopt) {
            auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestVideo(m_videoIds[i]);

            auto watcher = new QFutureWatcher<QInvidious::VideoResult>(this);
            connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, i] {
                auto result = watcher->result();

                if (const auto video = std::get_if<QInvidious::Video>(&result)) {
                    m_videoInfo[i] = *video;
                    Q_EMIT dataChanged(index(i, 0), index(i, 0));
                } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
                    qDebug() << "VideoQueue::requestMissingVideoInformation(): Error:" << error->second << error->first;
                }

                watcher->deleteLater();
            });
            watcher->setFuture(future);
        }
        i++;
    }
}

bool VideoQueue::shouldBeVisible() const
{
    return m_videoIds.size() > 1;
}

#include "moc_videoqueue.cpp"