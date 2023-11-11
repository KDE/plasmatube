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

    requestMissingVideoInformation();

    m_currentIndex = 0;
    Q_EMIT currentVideoChanged();
}

void VideoQueue::queueNext(const QString &videoId)
{
    beginInsertRows({}, m_videoIds.size(), m_videoIds.size() + 1);
    m_videoIds.push_back(videoId);
    m_videoInfo.push_back(std::nullopt);
    endInsertRows();

    requestMissingVideoInformation();
}

void VideoQueue::playInQueue(int index)
{
    // TODO: protect against invalid indexes
    m_currentIndex = index;
    Q_EMIT currentVideoChanged();
}

void VideoQueue::next()
{
    if (m_currentIndex + 1 < m_videoIds.size()) {
        m_currentIndex++;
        Q_EMIT currentVideoChanged();
    }
}

void VideoQueue::previous()
{
    if (m_currentIndex - 1 > 0) {
        m_currentIndex--;
        Q_EMIT currentVideoChanged();
    }
}

QString VideoQueue::getCurrentVideoId() const
{
    if (m_videoIds.isEmpty()) {
        return QString();
    }
    return m_videoIds[m_currentIndex];
}

int VideoQueue::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    qInfo() << "ROW COUNT:" << m_videoIds.size();
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
            {WatchedRole, "watched"}};
}

void VideoQueue::requestMissingVideoInformation()
{
    size_t i = 0;
    for (auto video : m_videoInfo) {
        // TODO: this doesn't cover the case where we could add another video while it's still fetchibng
        if (video == std::nullopt) {
            auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestVideo(m_videoIds[i]);

            m_watcher = new QFutureWatcher<QInvidious::VideoResult>(this);
            connect(m_watcher, &QFutureWatcherBase::finished, this, [this, i] {
                auto result = m_watcher->result();

                if (const auto video = std::get_if<QInvidious::Video>(&result)) {
                    qInfo() << "Recieved video information!";
                    m_videoInfo[i] = *video;
                    Q_EMIT dataChanged(index(i, 0), index(i, 0));
                } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
                    qDebug() << "VideoQueue::requestMissingVideoInformation(): Error:" << error->second << error->first;
                }

                m_watcher->deleteLater();
                m_watcher = nullptr;
            });
            m_watcher->setFuture(future);
        }
        i++;
    }
}

#include "moc_videoqueue.cpp"