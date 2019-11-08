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

#include <QVector>

#include "YTMThumbnailSet.h"

class QJsonObject;

class YTMSearchResult
{
public:
    class Item
    {
    public:
        static Item fromJson(const QJsonObject &json);

        QString title() const;
        void setTitle(const QString &);

        QVector<QString> attributes() const;
        void setAttributes(const QVector<QString> &attributes);

        YTMThumbnailSet thumbnails() const;
        void setThumbnails(const YTMThumbnailSet &thumbs);

    private:
        static QString attributeFromJson(const QJsonObject &json);

        QString m_title;
        QVector<QString> m_attributes;
        YTMThumbnailSet m_thumbnails;
    };

    class Shelf
    {
    public:
        static Shelf fromJson(const QJsonObject &json);

        QString title() const;
        void setTitle(const QString &);

        QVector<Item> contents() const;
        QVector<Item> &contents();
        void setContents(const QVector<YTMSearchResult::Item> &);

    private:
        QString m_title;
        QVector<Item> m_contents;
        // TODO: bottomText/bottomEndpoint ('More' action)
    };

    static YTMSearchResult fromJson(const QJsonObject &json);

    QVector<Shelf> contents() const;
    QVector<Shelf> &contents();
    void setContents(const QVector<Shelf> &);

private:
    QVector<Shelf> m_contents;
};

#endif // YTMSEARCHRESULT_H
