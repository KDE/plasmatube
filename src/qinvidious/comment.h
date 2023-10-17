// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qinvidiousglobal.h"

namespace QInvidious
{

class Comment
{
public:
    FROM_JSON_OVERLOADS(Comment)
    static Comment fromJson(const QJsonObject &, Comment &);

    QString author() const;
    void setAuthor(const QString &name);

    QString authorAvatar() const;
    void setAuthorAvatar(const QString &url);

    QString content() const;
    void setContent(const QString &content);

private:
    QString m_author;
    QString m_authorAvatar;
    QString m_content;
};

}
