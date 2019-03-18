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

#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractListModel>
#include "videobasicinfo.h"

class InvidiousManager;
class QNetworkReply;

class SearchModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery
               NOTIFY searchQueryChanged)

public:
    enum SearchResultRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ThumbnailRole,
        LengthRole,
        ViewCountRole,
        AuthorRole,
        AuthorIdRole,
        AuthorUrlRole,
        PublishedRole,
        PublishedTextRole,
        DescriptionRole,
        DescriptionHtmlRole,
        LiveNowRole,
        PaidRole,
        PremiumRole
    };

    explicit SearchModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE virtual void fetchMore(const QModelIndex &parent) override;
    Q_INVOKABLE virtual bool canFetchMore(const QModelIndex &parent) const override;

    bool isLoading() const;

    QString searchQuery() const;
    void setSearchQuery(const QString&);

signals:
    void isLoadingChanged();
    void searchQueryChanged();

private slots:
    void handleSearchResults(const QList<VideoBasicInfo>&);
    void handleSearchFailure();

private:
    void setIsLoading(bool);
    void clearAll();

    bool m_loading = false;
    QString m_searchQuery;
    qint32 m_nextPage = 0;

    QList<VideoBasicInfo> m_results;
    InvidiousManager *invidious;
    QNetworkReply *lastRequest = nullptr;
};

#endif // SEARCHMODEL_H
