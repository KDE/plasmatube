// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videothumbnail.h"

using namespace QInvidious;

VideoThumbnail VideoThumbnail::fromJson(const QJsonObject &obj, VideoThumbnail &thumb)
{
    thumb.setQuality(obj.value("quality").toString());
    thumb.setUrl(QUrl(obj.value("url").toString()));
    thumb.setWidth(obj.value("width").toInt());
    thumb.setHeight(obj.value("height").toInt());
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
