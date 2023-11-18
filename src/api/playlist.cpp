// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "playlist.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

Playlist Playlist::fromJson(const QJsonObject &obj, Playlist &playlist)
{
    const bool isPiped = obj.contains("uploaderName"_L1);
    if (isPiped) {
        playlist.m_id = obj["url"_L1].toString().remove(QStringLiteral("/playlist?list="));
        playlist.m_title = obj["name"_L1].toString();
        playlist.m_thumbnail = obj["thumbnail"_L1].toString();
        playlist.m_videoCount = obj["videos"_L1].toInt();
    } else {
        playlist.m_id = obj["playlistId"_L1].toString();
        playlist.m_title = obj["title"_L1].toString();
        const QJsonValue firstVideo = obj["videos"_L1].toArray().first();
        const QJsonValue firstVideoThumb = firstVideo.toObject()["videoThumbnails"_L1].toArray().first();
        playlist.m_thumbnail = firstVideoThumb.toObject()["url"_L1].toString();
        playlist.m_videoCount = obj["videoCount"_L1].toInt();
    }

    return playlist;
}

QString Playlist::id() const
{
    return m_id;
}

QString Playlist::title() const
{
    return m_title;
}

QString Playlist::thumbnail() const
{
    return m_thumbnail;
}

int Playlist::videoCount() const
{
    return m_videoCount;
}
