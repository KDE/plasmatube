// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstractlistmodel.h"

#include "plasmatube.h"

#include <KLocalizedString>

#include <QFutureWatcher>
#include <QtConcurrent>

AbstractListModel::AbstractListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> AbstractListModel::roleNames() const
{
    return {{IdRole, "id"},
            {TypeRole, "type"},
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
            {VideoCountRole, "videoCount"},
            {ChannelNameRole, "channelName"},
            {ChannelAvatarRole, "channelAvatar"}};
}

bool AbstractListModel::isLoading() const
{
    return m_loading;
}

void AbstractListModel::setLoading(const bool loading)
{
    if (loading != m_loading) {
        m_loading = loading;
        Q_EMIT isLoadingChanged();
    }
}

#include "moc_abstractlistmodel.cpp"