// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qinvidiousglobal.h"

namespace QInvidious
{

class Playlist
{
public:
    FROM_JSON_OVERLOADS(Playlist)
    static Playlist fromJson(const QJsonObject &, Playlist &);

    QString title() const;
    void setTitle(const QString &name);

    QString thumbnail() const;
    void setThumbnail(const QString &name);

    int videoCount() const;
    void setVideoCount(int count);

private:
    QString m_title;
    QString m_thumbnail;
    int m_videoCount;
};

}
