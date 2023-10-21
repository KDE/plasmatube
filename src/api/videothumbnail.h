// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qinvidiousglobal.h"

namespace QInvidious
{

class VideoThumbnail
{
public:
    FROM_JSON_OVERLOADS(VideoThumbnail)
    static VideoThumbnail fromJson(const QJsonObject &, VideoThumbnail &);

    QString quality() const;
    void setQuality(const QString &);

    QUrl url() const;
    void setUrl(const QUrl &);

    uint32_t width() const;
    void setWidth(uint32_t);

    uint32_t height() const;
    void setHeight(uint32_t);

private:
    QString m_quality;
    QUrl m_url;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

}
