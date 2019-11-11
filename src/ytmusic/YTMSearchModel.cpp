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
    roles[TypeRole] = QByteArrayLiteral("type");
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
    return m_items.length();
}

QVariant YTMSearchModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid | QAbstractItemModel::CheckIndexOption::ParentIsInvalid));

    switch (role) {
    case TypeRole:
        if (at(index).canConvert<YTMSearchResult::Item>())
            return Item;
        if (at(index).canConvert<YTMSearchResult::Shelf>())
            return Shelf;
        if (at(index).canConvert<YTMSearchRequest>())
            return SearchEndpoint;
        break;
    case ShelfTitleRole:
        return shelfAt(index).title();
    case TitleRole:
        return itemAt(index).title();
    case AttributesRole:
        return itemAt(index).attributes();
    case ThumbnailUrlRole:
        return itemAt(index).thumbnails().bestThumbnailForResolution(60).url();
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

bool YTMSearchModel::canGoBack() const
{
    return m_currentRequestIndex > 0;
}

bool YTMSearchModel::canGoForward() const
{
    return (m_currentRequestIndex + 1) < m_requestCache.length();
}

void YTMSearchModel::search(const YTMSearchRequest &request)
{
    // remove requests after current index
    m_requestCache.resize(m_currentRequestIndex + 1);
    m_requestCache << request;
    m_currentRequestIndex++;
}

void YTMSearchModel::search(const QString &query, const QString &params)
{
    search(YTMSearchRequest(query, params));
}

void YTMSearchModel::searchByEndpointIndex(int index)
{
    search(m_items.at(index).value<YTMSearchRequest>());
}

void YTMSearchModel::goBack()
{
    m_currentRequestIndex--;
}

void YTMSearchModel::goForward()
{
    m_currentRequestIndex++;
}

void YTMSearchModel::tryStartFetching()
{
    emit canGoBackChanged();
    emit canGoForwardChanged();

    setIsLoading(true);

    beginResetModel();
    m_items.clear();
    endResetModel();

    if (YTMClient::instance()->hasFetchedApiKey())
        fetchResults();
}

void YTMSearchModel::fetchResults()
{
    QNetworkReply *reply = YTMClient::instance()->sendRequest(m_requestCache.at(m_currentRequestIndex));

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        const auto result = YTMSearchResult::fromJson(QJsonDocument::fromJson(reply->readAll()).object());

        beginResetModel();
        m_items.clear();

        for (const auto &shelf : result.contents()) {
            m_items << QVariant::fromValue(shelf);

            for (const auto &item : shelf.contents())
                m_items << QVariant::fromValue(item);

            if (!shelf.bottomEndpoint().isNull())
                m_items << QVariant::fromValue(shelf.bottomEndpoint());
        }
        endResetModel();

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

QVariant YTMSearchModel::at(const QModelIndex &index) const
{
    return m_items.at(index.row());
}

YTMSearchResult::Item YTMSearchModel::itemAt(const QModelIndex &index) const
{
    return at(index).value<YTMSearchResult::Item>();
}

YTMSearchResult::Shelf YTMSearchModel::shelfAt(const QModelIndex &index) const
{
    return at(index).value<YTMSearchResult::Shelf>();
}
