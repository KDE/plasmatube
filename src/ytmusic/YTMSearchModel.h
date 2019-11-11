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

#ifndef YTMSEARCHMODEL_H
#define YTMSEARCHMODEL_H

#include <QAbstractListModel>

#include <QStack>

#include "YTMApiRequest.h"
#include "YTMSearchResult.h"
#include "YTMSearchRequest.h"

class YTMSearchModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY canGoBackChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY canGoForwardChanged)

public:
    enum ItemType {
        Shelf,
        Item,
        SearchEndpoint // e.g. 'show all' button
    };
    Q_ENUM(ItemType)

    enum Role {
        TypeRole = Qt::UserRole + 1,
        ShelfTitleRole,
        TitleRole,
        AttributesRole,
        ThumbnailUrlRole,
        VideoIdRole
    };

    YTMSearchModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool isLoading() const;
    bool canGoBack() const;
    bool canGoForward() const;

    Q_INVOKABLE void tryStartFetching();

    void search(const YTMSearchRequest &request);
    Q_INVOKABLE void search(const QString &query, const QString &params);
    Q_INVOKABLE void searchByEndpointIndex(int index);

    Q_INVOKABLE void goBack();
    Q_INVOKABLE void goForward();

signals:
    void isLoadingChanged();
    void canGoBackChanged();
    void canGoForwardChanged();

private:
    void fetchResults();

    void setResult(YTMSearchResult result);
    void setIsLoading(bool);

    QVariant at(const QModelIndex &index) const;
    YTMSearchResult::Item itemAt(const QModelIndex &index) const;
    YTMSearchResult::Shelf shelfAt(const QModelIndex &index) const;

    bool m_isLoading;

    QVector<QVariant> m_items;
    QStack<YTMSearchRequest> m_requestCache;
    int m_currentRequestIndex = -1;
};

#endif // YTMSEARCHMODEL_H
