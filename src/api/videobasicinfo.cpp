// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videobasicinfo.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

VideoBasicInfo VideoBasicInfo::fromJson(const QJsonObject &obj, VideoBasicInfo &info)
{
    const bool isPeerTube = obj.contains("id"_L1);
    const bool isInvidious = obj.contains("videoId"_L1);
    if (isPeerTube) {
        info.m_videoId = obj.value("uuid"_L1).toString();
        info.m_title = obj.value("name"_L1).toString();
        info.m_length = QTime(0, 0).addSecs(obj.value("duration"_L1).toInt());
        info.m_viewCount = obj.value("views"_L1).toInt();

        const auto channel = obj.value("channel"_L1).toObject();

        info.m_author = channel.value("displayName"_L1).toString();
        info.m_authorId = channel.value("name"_L1).toString();
        info.m_authorUrl = channel.value("url"_L1).toString();
        // FIXME: 2038 problem (timestamp is only 32 bit long)
        info.m_published = QDateTime::fromSecsSinceEpoch(obj.value("publishedAt"_L1).toInt());
        info.m_publishedText = info.published().toString();
        info.m_description = obj.value("description"_L1).toString();
        info.m_descriptionHtml = obj.value("description"_L1).toString();
        info.m_liveNow = obj.value("isLive"_L1).toBool(false);
        info.m_paid = false;
        info.m_premium = false;
        info.m_upcoming = false;

        VideoThumbnail thumbnail;
        thumbnail.setUrl(QUrl::fromUserInput(obj.value("thumbnailPath"_L1).toString()));
        info.m_videoThumbnails.push_back(thumbnail);
    } else if (isInvidious) {
        info.m_videoId = obj.value("videoId"_L1).toString();
        info.m_title = obj.value("title"_L1).toString();
        info.m_length = QTime(0, 0).addSecs(obj.value("lengthSeconds"_L1).toInt());
        info.m_viewCount = obj.value("viewCount"_L1).toInt();
        info.m_author = obj.value("author"_L1).toString();
        info.m_authorId = obj.value("authorId"_L1).toString();
        info.m_authorUrl = obj.value("authorUrl"_L1).toString();
        // FIXME: 2038 problem (timestamp is only 32 bit long)
        info.m_published = QDateTime::fromSecsSinceEpoch(obj.value("published"_L1).toInt());
        info.m_publishedText = info.published().toString();
        info.m_description = obj.value("description"_L1).toString();
        info.m_descriptionHtml = obj.value("descriptionHtml"_L1).toString();
        info.m_liveNow = obj.value("liveNow"_L1).toBool(false);
        info.m_paid = obj.value("paid"_L1).toBool(false);
        info.m_premium = obj.value("premium"_L1).toBool(false);
        info.m_upcoming = obj.value("isUpcoming"_L1).toBool(false);
        parseArray(obj.value("videoThumbnails"_L1), info.m_videoThumbnails);
        if (obj.contains("indexId"_L1)) {
            info.m_indexId = obj.value("indexId"_L1).toString();
        }
    } else {
        info.m_videoId = obj.value("url"_L1).toString().remove(QStringLiteral("/watch?v="));
        info.m_title = obj.value("title"_L1).toString();
        info.m_length = QTime(0, 0).addSecs(obj.value("duration"_L1).toInt());
        info.m_viewCount = obj.value("views"_L1).toInt();
        info.m_author = obj.value("uploaderName"_L1).toString();
        info.m_authorUrl = obj.value("uploaderUrl"_L1).toString();
        info.m_authorId = obj.value("uploaderUrl"_L1).toString().remove(QStringLiteral("/channel/"));
        // FIXME: 2038 problem (timestamp is only 32 bit long)
        info.m_published = QDateTime::fromSecsSinceEpoch(obj.value("uploaded"_L1).toInt());
        info.m_publishedText = obj.value("uploadedDate"_L1).toString();
        if (obj.contains("description"_L1)) {
            info.m_description = obj.value("description"_L1).toString();
            info.m_descriptionHtml = obj.value("description"_L1).toString();
        } else {
            info.m_description = obj.value("shortDescription"_L1).toString();
            info.m_descriptionHtml = obj.value("shortDescription"_L1).toString();
        }
        info.m_liveNow = false;
        info.m_paid = false;
        info.m_premium = false;
        info.m_upcoming = false;

        VideoThumbnail thumbnail;
        thumbnail.setUrl(QUrl::fromUserInput(obj.value("thumbnail"_L1).toString()));
        info.m_videoThumbnails.push_back(thumbnail);
    }
    return info;
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
    return {};
}

QTime VideoBasicInfo::length() const
{
    return m_length;
}

qint64 VideoBasicInfo::viewCount() const
{
    return m_viewCount;
}

QString VideoBasicInfo::author() const
{
    return m_author;
}

QString VideoBasicInfo::authorId() const
{
    return m_authorId;
}

QString VideoBasicInfo::authorUrl() const
{
    return m_authorUrl;
}

QDateTime VideoBasicInfo::published() const
{
    return m_published;
}

QString VideoBasicInfo::publishedText() const
{
    return m_publishedText;
}

QString VideoBasicInfo::description() const
{
    return m_description;
}

QString VideoBasicInfo::descriptionHtml() const
{
    return m_descriptionHtml;
}

bool VideoBasicInfo::liveNow() const
{
    return m_liveNow;
}

bool VideoBasicInfo::paid() const
{
    return m_paid;
}

bool VideoBasicInfo::premium() const
{
    return m_premium;
}

bool VideoBasicInfo::upcoming() const
{
    return m_upcoming;
}

QString VideoBasicInfo::indexId() const
{
    return m_indexId;
}
