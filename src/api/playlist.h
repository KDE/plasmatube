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

    QString id() const;

    QString title() const;

    QString thumbnail() const;

    int videoCount() const;

private:
    QString m_id;
    QString m_title;
    QString m_thumbnail;
    int m_videoCount;
};

}
