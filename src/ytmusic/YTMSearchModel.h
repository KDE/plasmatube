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

#include "YTMSearchResult.h"

class YTMSearchModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

public:
    enum Role {
        ShelfTitleRole = Qt::UserRole + 1,
        TitleRole,
        AttributesRole,
        ThumbnailUrlRole
    };

    YTMSearchModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool isLoading() const;

    QString query() const;
    void setQuery(const QString &);

signals:
    void isLoadingChanged();
    void queryChanged();

private:
    void fetchResults();

    void setResult(YTMSearchResult result);
    void setIsLoading(bool);

    YTMSearchResult::Shelf shelfAtIndex(int index) const;
    YTMSearchResult::Item itemAtIndex(int index) const;

    bool m_isLoading;
    QString m_query;
    YTMSearchResult m_result;
};

#endif // YTMSEARCHMODEL_H
