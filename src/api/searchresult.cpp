// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "searchresult.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

SearchResult SearchResult::fromJson(const QJsonObject &obj, SearchResult &info)
{
    const bool isPeerTube = obj.contains("shortUUID"_L1);

    // Piped calls regular videos "streams" for some reason
    if (isPeerTube || obj["type"_L1] == "video"_L1 || obj["type"_L1] == "stream"_L1) {
        info.m_type = Type::Video;
        info.m_videoBasicInfo = VideoBasicInfo::fromJson(obj);
    } else if (obj["type"_L1] == "channel"_L1) {
        info.m_type = Type::Channel;
        info.m_channel = Channel::fromJson(obj);
    } else if (obj["type"_L1] == "playlist"_L1) {
        info.m_type = Type::Playlist;
        info.m_playlist = Playlist::fromJson(obj);
    } else {
        qWarning() << "Cannot handle unknown result type" << obj["type"_L1].toString();
    }

    return info;
}

SearchResult::Type SearchResult::type() const
{
    return m_type;
}

VideoBasicInfo const &SearchResult::video() const
{
    Q_ASSERT(m_type == Type::Video);
    return *m_videoBasicInfo;
}

void SearchResult::setVideo(const VideoBasicInfo &video)
{
    m_videoBasicInfo = video;
}

Channel const &SearchResult::channel() const
{
    Q_ASSERT(m_type == Type::Channel);
    return *m_channel;
}

Playlist const &SearchResult::playlist() const
{
    Q_ASSERT(m_type == Type::Playlist);
    return *m_playlist;
}