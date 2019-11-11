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

#include "MediaFormat.h"

#include <QDateTime>
#include <QJsonObject>
#include <QUrl>
#include <QSharedData>

#include "constants.h"

class MediaFormatPrivate : public QSharedData
{
public:
    MediaFormatPrivate();

    int bitrate;
    QUrl url;
    int itag;
    QString type;
    qint64 size;
    QDateTime creationTime;
    qint16 fps;
    QString container;
    QString encoding;
    QString resolution;
    QString qualityLabel;
};

MediaFormatPrivate::MediaFormatPrivate()
    : bitrate(0),
      itag(0),
      size(0),
      fps(0)
{
}

MediaFormat::MediaFormat()
    : d(new MediaFormatPrivate)
{
}

MediaFormat::MediaFormat(const MediaFormat &other) = default;
MediaFormat::~MediaFormat() = default;
MediaFormat &MediaFormat::operator=(const MediaFormat &other) = default;

MediaFormat MediaFormat::fromJson(const QJsonObject &json)
{
    MediaFormat format;
    format.setBitrate(json.value(QStringLiteral("bitrate")).toInt());
    format.setUrl(json.value(QStringLiteral("url")).toString());
    format.setItag(json.value(QStringLiteral("itag")).toString().toInt());
    format.setType(json.value(QStringLiteral("type")).toString());
    format.setSize(json.value(QStringLiteral("clen")).toInt());
    format.setCreationTime(QDateTime::fromMSecsSinceEpoch(json.value(QStringLiteral("lmt")).toInt()));
    format.setFps(json.value(QStringLiteral("fps")).toInt());
    format.setContainer(json.value(QStringLiteral("container")).toString());
    format.setEncoding(json.value(QStringLiteral("encoding")).toString());
    format.setResolution(json.value(QStringLiteral("resolution")).toString());
    format.setQualityLabel(json.value(QStringLiteral("qualityLabel")).toString());

    return format;
}

CREATE_GETTER_AND_SETTER_BASETYPE(MediaFormat, int, d->bitrate, bitrate, setBitrate)
CREATE_GETTER_AND_SETTER(MediaFormat, QUrl, d->url, url, setUrl)
CREATE_GETTER_AND_SETTER_BASETYPE(MediaFormat, int, d->itag, itag, setItag)
CREATE_GETTER_AND_SETTER(MediaFormat, QString, d->type, type, setType)
CREATE_GETTER_AND_SETTER_BASETYPE(MediaFormat, qint64, d->size, size, setSize)
CREATE_GETTER_AND_SETTER(MediaFormat, QDateTime, d->creationTime, creationTime, setCreationTime)
CREATE_GETTER_AND_SETTER_BASETYPE(MediaFormat, qint16, d->fps, fps, setFps)
CREATE_GETTER_AND_SETTER(MediaFormat, QString, d->container, container, setContainer)
CREATE_GETTER_AND_SETTER(MediaFormat, QString, d->encoding, encoding, setEncoding)
CREATE_GETTER_AND_SETTER(MediaFormat, QString, d->resolution, resolution, setResolution)
CREATE_GETTER_AND_SETTER(MediaFormat, QString, d->qualityLabel, qualityLabel, setQualityLabel)
