// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "playlist.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

Playlist Playlist::fromJson(const QJsonObject &obj, Playlist &playlist)
{
    playlist.setId(obj["playlistId"_L1].toString());
    playlist.setTitle(obj["title"_L1].toString());
    const QJsonValue firstVideo = obj["videos"_L1].toArray().first();
    const QJsonValue firstVideoThumb = firstVideo.toObject()["videoThumbnails"_L1].toArray().first();
    playlist.setThumbnail(firstVideoThumb.toObject()["url"_L1].toString());
    playlist.setVideoCount(obj["videoCount"_L1].toInt());
    return playlist;
}

QString Playlist::id() const
{
    return m_id;
}

void Playlist::setId(const QString &id)
{
    m_id = id;
}

QString Playlist::title() const
{
    return m_title;
}

void Playlist::setTitle(const QString &name)
{
    m_title = name;
}

QString Playlist::thumbnail() const
{
    return m_thumbnail;
}

void Playlist::setThumbnail(const QString &url)
{
    m_thumbnail = url;
}

int Playlist::videoCount() const
{
    return m_videoCount;
}

void Playlist::setVideoCount(int count)
{
    m_videoCount = count;
}