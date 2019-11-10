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

#include "YTMSearchModel.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

#include "YTMClient.h"
#include "YTMSearchRequest.h"

YTMSearchModel::YTMSearchModel(QObject *parent)
    : QAbstractListModel(parent)
{
    if (!YTMClient::instance()->hasFetchedApiKey())
        YTMClient::instance()->fetchApiKey();
    
    connect(YTMClient::instance(), &YTMClient::apiKeyFetched, this, [=] (bool success) {
        if (m_isLoading) {
            if (success) {
                fetchResults();
            } else {
                setIsLoading(false);
                // TODO: error message: couldn't fetch api key
            }
        }
    });
}

QHash<int, QByteArray> YTMSearchModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ShelfTitleRole] = QByteArrayLiteral("shelfTitle");
    roles[TitleRole] = QByteArrayLiteral("title");
    roles[AttributesRole] = QByteArrayLiteral("attributes");
    roles[ThumbnailUrlRole] = QByteArrayLiteral("thumbnailUrl");
    return roles;
}

int YTMSearchModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    int count = 0;

    const QVector<YTMSearchResult::Shelf> &shelves = m_result.contents();
    for (const auto &shelf : shelves)
        count += shelf.contents().length();

    return count;
}

QVariant YTMSearchModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid | QAbstractItemModel::CheckIndexOption::ParentIsInvalid));

    if (role == ShelfTitleRole)
        return shelfAtIndex(index.row()).title();

    const auto &item = itemAtIndex(index.row());
    switch (role) {
    case TitleRole:
        return item.title();
    case AttributesRole:
        return item.attributes();
    case ThumbnailUrlRole:
        return item.thumbnails().bestThumbnailForResolution(120).url();
    default:
        return {};
    }

    Q_UNREACHABLE();
}

bool YTMSearchModel::isLoading() const
{
    return m_isLoading;
}

void YTMSearchModel::setIsLoading(bool isLoading)
{
    m_isLoading = isLoading;
    emit isLoadingChanged();
}

QString YTMSearchModel::query() const
{
    return m_query;
}

void YTMSearchModel::setQuery(const QString &query)
{
    m_query = query;
    emit queryChanged();

    setIsLoading(true);

    if (YTMClient::instance()->hasFetchedApiKey()) {
        fetchResults();
        return;
    }
}

void YTMSearchModel::setResult(YTMSearchResult result)
{
    beginResetModel();
    m_result = result;
    endResetModel();
}

void YTMSearchModel::fetchResults()
{
    QNetworkReply *reply = YTMClient::instance()->sendRequest(YTMSearchRequest(m_query));

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        setResult(YTMSearchResult::fromJson(QJsonDocument::fromJson(reply->readAll()).object()));
        setIsLoading(false);

        reply->deleteLater();
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError error) {
        qDebug() << "Couldn't fetch results:" << error;
        setIsLoading(false);
        // TODO: error message
        reply->deleteLater();
    });
}

YTMSearchResult::Shelf YTMSearchModel::shelfAtIndex(int index) const
{
    const QVector<YTMSearchResult::Shelf> &shelves = m_result.contents();
    for (const auto &shelf : shelves) {
        if (shelf.contents().length() > index)
            return shelf;

        index -= shelf.contents().length();
    }

    return {};
}

YTMSearchResult::Item YTMSearchModel::itemAtIndex(int index) const
{
    const QVector<YTMSearchResult::Shelf> &shelves = m_result.contents();
    for (const auto &shelf : shelves) {
        if (shelf.contents().length() > index)
            return shelf.contents().at(index);

        index -= shelf.contents().length();
    }

    return {};
}
