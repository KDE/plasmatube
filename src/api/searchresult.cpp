// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "searchresult.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

SearchResult SearchResult::fromJson(const QJsonObject &obj, SearchResult &info)
{
    SearchResult result;
    if (obj["type"_L1] == "video"_L1) {
        result.m_type = Type::Video;
        result.m_videoBasicInfo = VideoBasicInfo::fromJson(obj);
    } else if (obj["type"_L1] == "channel"_L1) {
        result.m_type = Type::Channel;
        result.m_channel = Channel::fromJson(obj);
    } else if (obj["type"_L1] == "playlist"_L1) {
        result.m_type = Type::Playlist;
        result.m_playlist = Playlist::fromJson(obj);
    } else {
        qWarning() << "Cannot handle unknown result type" << obj["type"_L1].toString();
    }

    return result;
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