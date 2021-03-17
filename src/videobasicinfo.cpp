/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "videobasicinfo.h"
#include <QJsonArray>

void VideoThumbnail::parseFromJson(const QJsonObject &obj)
{
    setQuality(obj.value("quality").toString());
    setUrl(QUrl(obj.value("url").toString()));
    setWidth(obj.value("width").toInt());
    setHeight(obj.value("height").toInt());
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

qint32 VideoThumbnail::width() const
{
    return m_width;
}

void VideoThumbnail::setWidth(qint32 width)
{
    m_width = width;
}

qint32 VideoThumbnail::height() const
{
    return m_height;
}

void VideoThumbnail::setHeight(qint32 height)
{
    m_height = height;
}

void VideoBasicInfo::parseFromJson(const QJsonObject &obj)
{
    setVideoId(obj.value("videoId").toString());
    setTitle(obj.value("title").toString());
    setLength(QTime(0, 0).addSecs(obj.value("lengthSeconds").toInt()));
    setViewCount(obj.value("viewCount").toInt());
    setAuthor(obj.value("author").toString());
    setAuthorId(obj.value("authorId").toString());
    setAuthorUrl(obj.value("authorUrl").toString());
    // FIXME: 2038 problem (timestamp is only 32 bit long)
    QDateTime published;
    published.setSecsSinceEpoch(obj.value("published").toInt());
    setPublished(published);
    setPublishedText(obj.value("publishedText").toString());
    setDescription(obj.value("description").toString());
    setDescriptionHtml(obj.value("descriptionHtml").toString());
    setLiveNow(obj.value("liveNow").toBool(false));
    setPaid(obj.value("paid").toBool(false));
    setPremium(obj.value("premium").toBool(false));

    const auto thumbs = obj.value("videoThumbnails").toArray();
    std::transform(thumbs.cbegin(), thumbs.cend(), std::back_inserter(m_videoThumbnails),
                   [](const QJsonValue &val) {
        VideoThumbnail thumb;
        thumb.parseFromJson(val.toObject());
        return thumb;
    });
}

bool VideoBasicInfo::isNotification() const
{
    return m_isNotification;
}

void VideoBasicInfo::setIsNotification(bool isNotification)
{
    m_isNotification = isNotification;
}

QString VideoBasicInfo::videoId() const
{
    return m_videoId;
}

void VideoBasicInfo::setVideoId(const QString &videoId)
{
    m_videoId = videoId;
}

QString VideoBasicInfo::title() const
{
    return m_title;
}

void VideoBasicInfo::setTitle(const QString &title)
{
    m_title = title;
}

QList<VideoThumbnail> VideoBasicInfo::videoThumbnails() const
{
    return m_videoThumbnails;
}

void VideoBasicInfo::setVideoThumbnails(const QList<VideoThumbnail> &videoThumbnails)
{
    m_videoThumbnails = videoThumbnails;
}

VideoThumbnail VideoBasicInfo::thumbnail(const QString &quality) const
{
    for (auto &thumb : m_videoThumbnails) {
        if (thumb.quality() == quality)
            return thumb;
    }
    // didn't found requested quality
    if (!m_videoThumbnails.isEmpty())
        return m_videoThumbnails.last();
    return VideoThumbnail();
}

QTime VideoBasicInfo::length() const
{
    return m_length;
}

void VideoBasicInfo::setLength(const QTime &length)
{
    m_length = length;
}

qint64 VideoBasicInfo::viewCount() const
{
    return m_viewCount;
}

void VideoBasicInfo::setViewCount(qint64 viewCount)
{
    m_viewCount = viewCount;
}

QString VideoBasicInfo::author() const
{
    return m_author;
}

void VideoBasicInfo::setAuthor(const QString &author)
{
    m_author = author;
}

QString VideoBasicInfo::authorId() const
{
    return m_authorId;
}

void VideoBasicInfo::setAuthorId(const QString &authorId)
{
    m_authorId = authorId;
}

QString VideoBasicInfo::authorUrl() const
{
    return m_authorUrl;
}

void VideoBasicInfo::setAuthorUrl(const QString &authorUrl)
{
    m_authorUrl = authorUrl;
}

QDateTime VideoBasicInfo::published() const
{
    return m_published;
}

void VideoBasicInfo::setPublished(const QDateTime &published)
{
    m_published = published;
}

QString VideoBasicInfo::publishedText() const
{
    return m_publishedText;
}

void VideoBasicInfo::setPublishedText(const QString &publishedText)
{
    m_publishedText = publishedText;
}

QString VideoBasicInfo::description() const
{
    return m_description;
}

void VideoBasicInfo::setDescription(const QString &description)
{
    m_description = description;
}

QString VideoBasicInfo::descriptionHtml() const
{
    return  m_descriptionHtml;
}

void VideoBasicInfo::setDescriptionHtml(const QString &descriptionHtml)
{
    m_descriptionHtml = descriptionHtml;
}

bool VideoBasicInfo::liveNow() const
{
    return m_liveNow;
}

void VideoBasicInfo::setLiveNow(bool liveNow)
{
    m_liveNow = liveNow;
}

bool VideoBasicInfo::paid() const
{
    return m_paid;
}

void VideoBasicInfo::setPaid(bool paid)
{
    m_paid = paid;
}

bool VideoBasicInfo::premium() const
{
    return m_premium;
}

void VideoBasicInfo::setPremium(bool premium)
{
    m_premium = premium;
}
