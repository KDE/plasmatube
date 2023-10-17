// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "comment.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

Comment Comment::fromJson(const QJsonObject &obj, Comment &comment)
{
    comment.setAuthor(obj["author"_L1].toString());
    comment.setContent(obj["contentHtml"_L1].toString());
    const QJsonValue firstAvatar = obj["authorThumbnails"_L1].toArray().first();
    comment.setAuthorAvatar(firstAvatar.toObject()["url"_L1].toString());
    return comment;
}

QString Comment::author() const
{
    return m_author;
}

void Comment::setAuthor(const QString &name)
{
    m_author = name;
}

QString Comment::authorAvatar() const
{
    return m_authorAvatar;
}

void Comment::setAuthorAvatar(const QString &url)
{
    m_authorAvatar = url;
}

QString Comment::content() const
{
    return m_content;
}

void Comment::setContent(const QString &content)
{
    m_content = content;
}
