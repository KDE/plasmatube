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

#ifndef YTMSEARCHRESULT_H
#define YTMSEARCHRESULT_H

#include <QMetaType>
#include <QVector>
#include <QSharedDataPointer>

#include "YTMThumbnailSet.h"
#include "YTMSearchRequest.h"

class QJsonObject;

class YTMSearchResultPrivate;
class YTMSearchResultItemPrivate;
class YTMSearchResultShelfPrivate;

class YTMSearchResult
{
public:
    class Item
    {
    public:
        static Item fromJson(const QJsonObject &json);

        Item();
        Item(const Item &other);
        ~Item();

        Item &operator=(const Item &other);

        QString title() const;
        void setTitle(const QString &);

        QStringList attributes() const;
        void setAttributes(const QStringList &attributes);

        YTMThumbnailSet thumbnails() const;
        void setThumbnails(const YTMThumbnailSet &thumbs);

        QString videoId() const;
        void setVideoId(const QString &videoId);

    private:
        static QString attributeFromJson(const QJsonObject &json);

        QSharedDataPointer<YTMSearchResultItemPrivate> d;
    };

    class Shelf
    {
    public:
        static Shelf fromJson(const QJsonObject &json);

        Shelf();
        Shelf(const Shelf &other);
        ~Shelf();

        Shelf &operator=(const Shelf &other);

        QString title() const;
        void setTitle(const QString &);

        QVector<Item> contents() const;
        QVector<Item> &contents();
        void setContents(const QVector<YTMSearchResult::Item> &);

        YTMSearchRequest bottomEndpoint() const;
        void setBottomEndpoint(const YTMSearchRequest &endpoint);

    private:
        QSharedDataPointer<YTMSearchResultShelfPrivate> d;
    };

    static YTMSearchResult fromJson(const QJsonObject &json);

    YTMSearchResult();
    YTMSearchResult(const YTMSearchResult &other);
    ~YTMSearchResult();

    YTMSearchResult &operator=(const YTMSearchResult &other);

    QVector<Shelf> contents() const;
    QVector<Shelf> &contents();
    void setContents(const QVector<Shelf> &);

private:
    QSharedDataPointer<YTMSearchResultPrivate> d;
};

Q_DECLARE_METATYPE(YTMSearchResult);
Q_DECLARE_METATYPE(YTMSearchResult::Item);
Q_DECLARE_METATYPE(YTMSearchResult::Shelf);

#endif // YTMSEARCHRESULT_H
