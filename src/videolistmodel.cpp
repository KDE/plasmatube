// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videolistmodel.h"
#include "invidiousmanager.h"

#include <QNetworkReply>

VideoListModel::VideoListModel(QObject *parent)
    : QAbstractListModel(parent),
      m_invidious(new InvidiousManager(this))
{
    connect(m_invidious, &InvidiousManager::videoQueryResults,
            this, &VideoListModel::handleSearchResults);
    connect(m_invidious, &InvidiousManager::videoQueryFailed,
            this, &VideoListModel::handleSearchFailure);
}

VideoListModel::VideoListModel(const QList<VideoBasicInfo> &list,
                               QObject *parent)
    : QAbstractListModel(parent), m_constant(true), m_results(list)
{
}

QHash<int, QByteArray> VideoListModel::roleNames() const
{
    return {
        {IdRole, "id"},
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
        {PremiumRole, "premium"}
    };
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
        m_lastRequest->abort();
        m_lastRequest->deleteLater();
        setIsLoading(false);
    }

    // clean up
    m_nextPage = 0;
    clearAll();

    setIsLoading(true);
    m_lastRequest = m_invidious->videoQuery(m_queryType, m_query, m_nextPage);
}

void VideoListModel::fetchMore(const QModelIndex&)
{
    if (!canFetchMore(QModelIndex()))
        return;

    setIsLoading(true);
    // only searches canFetchMore()
    m_lastRequest = m_invidious->videoQuery(m_queryType, m_query, m_nextPage);
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
    beginResetModel();
    m_results.clear();
    endResetModel();
}
