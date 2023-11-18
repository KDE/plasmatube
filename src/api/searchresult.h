// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "channel.h"
#include "playlist.h"
#include "qinvidiousglobal.h"
#include "videobasicinfo.h"
#include "videothumbnail.h"

#include <QDateTime>
#include <QUrl>

namespace QInvidious
{

class SearchResult
{
public:
    FROM_JSON_OVERLOADS(SearchResult)
    static SearchResult fromJson(const QJsonObject &, SearchResult &);

    // TODO: maybe remove this and expose the std::optional, which might look cleaner?
    enum class Type { Video, Channel, Playlist };

    Type type() const;

    VideoBasicInfo const &video() const;

    Channel const &channel() const;

    Playlist const &playlist() const;

private:
    std::optional<VideoBasicInfo> m_videoBasicInfo;
    std::optional<Channel> m_channel;
    std::optional<Playlist> m_playlist;
    Type m_type = Type::Video;
};

}
