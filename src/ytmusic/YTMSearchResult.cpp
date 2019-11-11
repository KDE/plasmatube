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
#include <QSharedData>

#include "constants.h"

class YTMSearchResultItemPrivate : public QSharedData
{
public:
    QString title;
    QStringList attributes;
    YTMThumbnailSet thumbnails;
    QString videoId;
};

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
            QStringList attributes;
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

    item.setVideoId(
        json.value(QStringLiteral("doubleTapCommand")).toObject()
            .value(QStringLiteral("watchEndpoint")).toObject()
            .value(QStringLiteral("videoId")).toString()
    );

    return item;
}

YTMSearchResult::Item::Item()
    : d(new YTMSearchResultItemPrivate)
{
}

YTMSearchResult::Item::Item(const Item &other) = default;
YTMSearchResult::Item::~Item() = default;

QString YTMSearchResult::Item::attributeFromJson(const QJsonObject &json)
{
    return json.value(QStringLiteral("musicResponsiveListItemFlexColumnRenderer")).toObject()
               .value(QStringLiteral("text")).toObject()
               .value(QStringLiteral("runs")).toArray()
               .first().toObject()
               .value(QStringLiteral("text")).toString();
}

CREATE_GETTER_AND_SETTER(YTMSearchResult::Item, QString, d->title, title, setTitle)
CREATE_GETTER_AND_SETTER(YTMSearchResult::Item, QStringList, d->attributes, attributes, setAttributes)
CREATE_GETTER_AND_SETTER(YTMSearchResult::Item, YTMThumbnailSet, d->thumbnails, thumbnails, setThumbnails)
CREATE_GETTER_AND_SETTER(YTMSearchResult::Item, QString, d->videoId, videoId, setVideoId)

class YTMSearchResultShelfPrivate : public QSharedData
{
public:
    QString title;
    QVector<YTMSearchResult::Item> contents;
    YTMSearchRequest bottomEndpoint;
};

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

    shelf.setBottomEndpoint(YTMSearchRequest::fromJson(
        json.value(QStringLiteral("bottomEndpoint")).toObject().value(QStringLiteral("searchEndpoint")).toObject()
    ));

    return shelf;
}

YTMSearchResult::Shelf::Shelf()
    : d(new YTMSearchResultShelfPrivate)
{
}

YTMSearchResult::Shelf::Shelf(const Shelf &other) = default;
YTMSearchResult::Shelf::~Shelf() = default;

YTMSearchResult::Shelf &YTMSearchResult::Shelf::operator=(const Shelf &other) = default;

CREATE_GETTER_AND_SETTER(YTMSearchResult::Shelf, QString, d->title, title, setTitle)
CREATE_GETTER_AND_SETTER(YTMSearchResult::Shelf, QVector<YTMSearchResult::Item>, d->contents, contents, setContents)
CREATE_GETTER_AND_SETTER(YTMSearchResult::Shelf, YTMSearchRequest, d->bottomEndpoint, bottomEndpoint, setBottomEndpoint)

QVector<YTMSearchResult::Item> & YTMSearchResult::Shelf::contents()
{
    return d->contents;
}

class YTMSearchResultPrivate : public QSharedData
{
public:
    QVector<YTMSearchResult::Shelf> contents;
};

YTMSearchResult YTMSearchResult::fromJson(const QJsonObject &json)
{
    YTMSearchResult result;

    const QJsonArray contents =
        json.value(QStringLiteral("contents")).toObject()
            .value(QStringLiteral("sectionListRenderer")).toObject()
            .value(QStringLiteral("contents")).toArray();
    
    for (const auto &value : contents) {
        result.d->contents << Shelf::fromJson(
            value.toObject().value(QStringLiteral("musicShelfRenderer")).toObject()
        );
    }

    return result;
}

YTMSearchResult::YTMSearchResult()
    : d(new YTMSearchResultPrivate)
{
}

YTMSearchResult::YTMSearchResult(const YTMSearchResult &other) = default;
YTMSearchResult::~YTMSearchResult() = default;

YTMSearchResult &YTMSearchResult::operator=(const YTMSearchResult &other) = default;

CREATE_GETTER_AND_SETTER(YTMSearchResult, QVector<YTMSearchResult::Shelf>, d->contents, contents, setContents)

QVector<YTMSearchResult::Shelf> &YTMSearchResult::contents()
{
    return d->contents;
}
