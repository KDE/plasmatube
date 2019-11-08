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

#include "YTMSearchResult.h"

#include <QJsonArray>
#include <QJsonObject>

YTMSearchResult::Item YTMSearchResult::Item::fromJson(const QJsonObject &json)
{
    Item item;

    // title / attributes
    const QJsonArray flexColumns = json.value(QStringLiteral("flexColumns")).toArray();
    if (!flexColumns.isEmpty()) {
        // the title is just the first attribute
        item.setTitle(attributeFromJson(flexColumns.first().toObject()));

        // other attributes
        if (flexColumns.size() > 1) {
            QVector<QString> attributes;
            attributes.reserve(flexColumns.size() - 1);

            std::for_each(flexColumns.begin() + 1, flexColumns.end(), [&] (const QJsonValue &value) {
                attributes << attributeFromJson(value.toObject());
            });

            item.setAttributes(attributes);
        }
    }

    item.setThumbnails(YTMThumbnailSet::fromJson(
        json.value(QStringLiteral("thumbnail")).toObject()
            .value(QStringLiteral("musicThumbnailRenderer")).toObject()
            .value(QStringLiteral("thumbnail")).toObject()
    ));

    return item;
}

QString YTMSearchResult::Item::attributeFromJson(const QJsonObject &json)
{
    return json.value(QStringLiteral("musicResponsiveListItemFlexColumnRenderer")).toObject()
               .value(QStringLiteral("text")).toObject()
               .value(QStringLiteral("runs")).toArray()
               .first().toObject()
               .value(QStringLiteral("text")).toString();
}

QString YTMSearchResult::Item::title() const
{
    return m_title;
}

void YTMSearchResult::Item::setTitle(const QString &title)
{
    m_title = title;
}

QVector<QString> YTMSearchResult::Item::attributes() const
{
    return m_attributes;
}

void YTMSearchResult::Item::setAttributes(const QVector<QString> &attributes)
{
    m_attributes = attributes;
}

YTMThumbnailSet YTMSearchResult::Item::thumbnails() const
{
    return m_thumbnails;
}

void YTMSearchResult::Item::setThumbnails(const YTMThumbnailSet &thumbs)
{
    m_thumbnails = thumbs;
}

YTMSearchResult::Shelf YTMSearchResult::Shelf::fromJson(const QJsonObject &json)
{
    Shelf shelf;
    shelf.setTitle(
        json.value(QStringLiteral("title")).toObject()
            .value(QStringLiteral("runs")).toArray()
            .first().toObject()
            .value(QStringLiteral("text")).toString()
    );

    const QJsonArray itemsArray = json.value(QStringLiteral("contents")).toArray();
    for (const auto &item : itemsArray) {
        shelf.contents() << YTMSearchResult::Item::fromJson(
            item.toObject().value(QStringLiteral("musicResponsiveListItemRenderer")).toObject()
        );
    }
    
    return shelf;
}

QString YTMSearchResult::Shelf::title() const
{
    return m_title;
}

void YTMSearchResult::Shelf::setTitle(const QString &title)
{
    m_title = title;
}

QVector<YTMSearchResult::Item> YTMSearchResult::Shelf::contents() const
{
    return m_contents;
}

QVector<YTMSearchResult::Item> & YTMSearchResult::Shelf::contents()
{
    return m_contents;
}

void YTMSearchResult::Shelf::setContents(const QVector<YTMSearchResult::Item> &contents)
{
    m_contents = contents;
}

YTMSearchResult YTMSearchResult::fromJson(const QJsonObject &json)
{
    YTMSearchResult result;

    const QJsonArray contents =
        json.value(QStringLiteral("contents")).toObject()
            .value(QStringLiteral("sectionListRenderer")).toObject()
            .value(QStringLiteral("contents")).toArray();
    
    for (const auto &value : contents) {
        result.m_contents << Shelf::fromJson(
            value.toObject().value(QStringLiteral("musicShelfRenderer")).toObject()
        );
    }

    return result;
}

QVector<YTMSearchResult::Shelf> YTMSearchResult::contents() const
{
    return m_contents;
}

QVector<YTMSearchResult::Shelf> & YTMSearchResult::contents()
{
    return m_contents;
}

void YTMSearchResult::setContents(const QVector<YTMSearchResult::Shelf> &contents)
{
    m_contents = contents;
}
