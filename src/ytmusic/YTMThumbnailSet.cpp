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

#include "YTMThumbnailSet.h"

#include <QJsonArray>
#include <QJsonObject>

#define JSON_THUMBNAILS "thumbnails"

YTMThumbnailSet YTMThumbnailSet::fromJson(const QJsonObject &json)
{
    YTMThumbnailSet thumbs;

    for (const auto &thumb : json.value(QStringLiteral(JSON_THUMBNAILS)).toArray()) {
        if (thumb.isObject())
            thumbs << YTMThumbnail::fromJson(thumb.toObject());
    }

    return thumbs;
}

QJsonObject YTMThumbnailSet::toJson() const
{
    QJsonObject json;

    QJsonArray thumbsArray;
    for (const auto &thumb : qAsConst(m_thumbnails))
        thumbsArray << thumb.toJson();

    json.insert(QStringLiteral(JSON_THUMBNAILS), thumbsArray);

    return json;
}

QSet<YTMThumbnail> YTMThumbnailSet::thumbnails() const
{
    return m_thumbnails;
}

QSet<YTMThumbnail> &YTMThumbnailSet::thumbnails()
{
    return m_thumbnails;
}

void YTMThumbnailSet::setThumbnails(const QSet<YTMThumbnail> &thumbnails)
{
    m_thumbnails = thumbnails;
}

void YTMThumbnailSet::insert(const YTMThumbnail &thumb)
{
    m_thumbnails << YTMThumbnail(thumb);
}

YTMThumbnail YTMThumbnailSet::bestThumbnailForResolution(qint32 width) const
{
    YTMThumbnail current;

    for (const auto &thumb : qAsConst(m_thumbnails)) {
        if ((thumb.size().width() > current.size().width() && current.size().width() <= width) ||
            (thumb.size().width() <= current.size().width() && thumb.size().width() >= width)) {
            current = thumb;
        }
    }

    return current;
}
