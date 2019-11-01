/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "videolistmodel.h"
#include "invidiousmanager.h"
#include <QNetworkReply>

VideoListModel::VideoListModel(QObject *parent)
    : QAbstractListModel(parent),
      invidious(new InvidiousManager("https://invidio.us", this))
{
    connect(invidious, &InvidiousManager::videoQueryResults,
            this, &VideoListModel::handleSearchResults);
    connect(invidious, &InvidiousManager::videoQueryFailed,
            this, &VideoListModel::handleSearchFailure);
}

VideoListModel::VideoListModel(const QList<VideoBasicInfo> &list,
                               QObject *parent)
    : QAbstractListModel(parent), m_constant(true), m_results(list)
{
}

QHash<int, QByteArray> VideoListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TitleRole] = "title";
    roles[ThumbnailRole] = "thumbnail";
    roles[LengthRole] = "length";
    roles[ViewCountRole] = "viewCount";
    roles[AuthorRole] = "author";
    roles[AuthorIdRole] = "authorId";
    roles[AuthorUrlRole] = "authorUrl";
    roles[PublishedRole] = "published";
    roles[PublishedTextRole] = "publishedText";
    roles[DescriptionRole] = "description";
    roles[DescriptionHtmlRole] = "descriptionHtml";
    roles[LiveNowRole] = "liveNow";
    roles[PaidRole] = "paid";
    roles[PremiumRole] = "premium";
    return roles;
}

int VideoListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_results.length();
}

QVariant VideoListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const VideoBasicInfo &video = m_results.at(index.row());
    switch (role) {
    case IdRole:
        return video.videoId();
    case TitleRole:
        return video.title();
    case ThumbnailRole:
        return video.thumbnail("medium").url();
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
    }
    return {};
}

void VideoListModel::fetch()
{
    // if currently loading, abort
    if (m_loading) {
        lastRequest->abort();
        lastRequest->deleteLater();
        setIsLoading(false);
    }

    // clean up
    m_nextPage = 0;
    clearAll();

    setIsLoading(true);
    lastRequest = invidious->videoQuery(m_queryType, m_query, m_nextPage);
}

void VideoListModel::fetchMore(const QModelIndex&)
{
    if (!canFetchMore(QModelIndex()))
        return;

    setIsLoading(true);
    // only searches canFetchMore()
    lastRequest = invidious->videoQuery(m_queryType, m_query, m_nextPage);
}

bool VideoListModel::canFetchMore(const QModelIndex &) const
{
    return !m_loading &&
            m_queryType == InvidiousManager::Search &&
           !m_constant;
}

bool VideoListModel::isLoading() const
{
    return m_loading;
}

InvidiousManager::VideoListType VideoListModel::queryType() const
{
    return m_queryType;
}

void VideoListModel::setQueryType(InvidiousManager::VideoListType queryType)
{
    m_queryType = queryType;
    emit queryTypeChanged();
}

QString VideoListModel::query() const
{
    return m_query;
}

void VideoListModel::setQuery(const QString &query)
{
    m_query = query;
    emit queryChanged();
}

void VideoListModel::handleSearchResults(const QList<VideoBasicInfo> &results)
{
    if (m_queryType != InvidiousManager::Trending)
        m_nextPage++;

    setIsLoading(false);
    int rows = rowCount();
    beginInsertRows(QModelIndex(), rows, rows + results.length() - 1);
    m_results.append(results);
    endInsertRows();
}

void VideoListModel::handleSearchFailure()
{
    setIsLoading(false);
}

void VideoListModel::setIsLoading(bool loading)
{
    m_loading = loading;
    emit isLoadingChanged();
}

void VideoListModel::clearAll()
{
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    m_results.clear();
    endRemoveRows();
}
