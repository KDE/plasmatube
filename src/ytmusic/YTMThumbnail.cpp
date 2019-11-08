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

#include "YTMThumbnail.h"

#include <QJsonObject>

YTMThumbnail::YTMThumbnail() = default;

YTMThumbnail::YTMThumbnail(const YTMThumbnail &other) = default;

YTMThumbnail::~YTMThumbnail() = default;

YTMThumbnail &YTMThumbnail::operator=(const YTMThumbnail& other) = default;

YTMThumbnail YTMThumbnail::fromJson(const QJsonObject &json)
{
    YTMThumbnail thumb;
    thumb.setUrl({json.value(QStringLiteral("url")).toString()});
    thumb.size().setWidth(json.value(QStringLiteral("width")).toInt());
    thumb.size().setHeight(json.value(QStringLiteral("height")).toInt());

    return thumb;
}

QJsonObject YTMThumbnail::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("url"), m_url.toString(QUrl::FullyEncoded));
    json.insert(QStringLiteral("width"), m_size.width());
    json.insert(QStringLiteral("height"), m_size.height());

    return json;
}

QSize YTMThumbnail::size() const
{
    return m_size;
}

QSize &YTMThumbnail::size()
{
    return m_size;
}

void YTMThumbnail::setSize(const QSize &size)
{
    m_size = size;
}

QUrl YTMThumbnail::url() const
{
    return m_url;
}

void YTMThumbnail::setUrl(const QUrl &url)
{
    m_url = url;
}

bool YTMThumbnail::isNull() const
{
    return m_url.isEmpty() && m_size.isNull();
}

bool YTMThumbnail::operator==(const YTMThumbnail& other) const
{
    return m_size == other.size() &&
           m_url == other.url();
}

uint qHash(const YTMThumbnail &thumb)
{
    return qHash(thumb.url()) ^ qHash(thumb.size().width());
}
