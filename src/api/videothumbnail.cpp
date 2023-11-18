// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videothumbnail.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

VideoThumbnail VideoThumbnail::fromJson(const QJsonObject &obj, VideoThumbnail &thumb)
{
    thumb.setQuality(obj.value("quality"_L1).toString());
    thumb.setUrl(QUrl(obj.value("url"_L1).toString()));
    thumb.setWidth(obj.value("width"_L1).toInt());
    thumb.setHeight(obj.value("height"_L1).toInt());
    return thumb;
}

QString VideoThumbnail::quality() const
{
    return m_quality;
}

void VideoThumbnail::setQuality(const QString &quality)
{
    m_quality = quality;
}

QUrl VideoThumbnail::url() const
{
    return m_url;
}

void VideoThumbnail::setUrl(const QUrl &url)
{
    m_url = url;
}

uint32_t VideoThumbnail::width() const
{
    return m_width;
}

void VideoThumbnail::setWidth(uint32_t width)
{
    m_width = width;
}

uint32_t VideoThumbnail::height() const
{
    return m_height;
}

void VideoThumbnail::setHeight(uint32_t height)
{
    m_height = height;
}
