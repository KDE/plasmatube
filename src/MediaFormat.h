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

#ifndef MEDIAFORMAT_H
#define MEDIAFORMAT_H

#include <QSharedDataPointer>

#include "constants.h"

class MediaFormatPrivate;
class QDateTime;
class QJsonObject;
class QUrl;

class MediaFormat
{
public:
    MediaFormat();
    MediaFormat(const MediaFormat &other);
    ~MediaFormat();

    MediaFormat &operator=(const MediaFormat &other);

    static MediaFormat fromJson(const QJsonObject &json);

    CREATE_GETTER_AND_SETTER_BASETYPE_HEADER(int, bitrate, setBitrate)
    CREATE_GETTER_AND_SETTER_HEADER(QUrl, url, setUrl)
    CREATE_GETTER_AND_SETTER_BASETYPE_HEADER(int, itag, setItag)
    CREATE_GETTER_AND_SETTER_HEADER(QString, type, setType)
    CREATE_GETTER_AND_SETTER_BASETYPE_HEADER(qint64, size, setSize)
    CREATE_GETTER_AND_SETTER_HEADER(QDateTime, creationTime, setCreationTime)
    CREATE_GETTER_AND_SETTER_BASETYPE_HEADER(qint16, fps, setFps)
    CREATE_GETTER_AND_SETTER_HEADER(QString, container, setContainer)
    CREATE_GETTER_AND_SETTER_HEADER(QString, encoding, setEncoding)
    CREATE_GETTER_AND_SETTER_HEADER(QString, resolution, setResolution)
    CREATE_GETTER_AND_SETTER_HEADER(QString, qualityLabel, setQualityLabel)

private:
    QSharedDataPointer<MediaFormatPrivate> d;
};

#endif // MEDIAFORMAT_H
