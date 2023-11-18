// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "comment.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

Comment Comment::fromJson(const QJsonObject &obj, Comment &comment)
{
    const bool isPeerTube = obj.contains("id"_L1);
    const bool isPiped = obj.contains("commentId"_L1) && !obj.contains("authorThumbnails"_L1);
    if (isPeerTube) {
        comment.m_author = obj["account"_L1].toObject()["displayName"_L1].toString();
        comment.m_content = obj["text"_L1].toString();
    } else if (isPiped) {
        comment.m_author = obj["author"_L1].toString();
        comment.m_content = obj["commentText"_L1].toString();
        comment.m_authorAvatar = obj["thumbnail"_L1].toString();
    } else {
        comment.m_author = obj["author"_L1].toString();
        comment.m_content = obj["contentHtml"_L1].toString();
        const QJsonValue firstAvatar = obj["authorThumbnails"_L1].toArray().first();
        comment.m_authorAvatar = firstAvatar.toObject()["url"_L1].toString();
    }
    return comment;
}

QString Comment::author() const
{
    return m_author;
}

QString Comment::authorAvatar() const
{
    return m_authorAvatar;
}

QString Comment::content() const
{
    return m_content;
}
