// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "video.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

Video Video::fromJson(const QJsonObject &obj, Video &video)
{
    VideoBasicInfo::fromJson(obj, video);

    const bool isPeerTube = obj.contains("id"_L1);
    const bool isInvidious = obj.contains("videoId"_L1);

    if (isInvidious) {
        parseArray(obj.value("keywords"_L1), video.m_keywords);
        video.m_likeCount = obj.value("likeCount"_L1).toInt();
        video.m_dislikeCount = obj.value("dislikeCount"_L1).toInt();
        video.m_isFamilyFriendly = obj.value("isFamilyFriendly"_L1).toBool(true);
        parseArray(obj.value("allowedRegions"_L1), video.m_allowedRegions);
        video.m_genre = obj.value("genre"_L1).toString();
        video.m_genreUrl = obj.value("genreUrl"_L1).toString();
        parseArray(obj.value("authorThumbnails"_L1), video.m_authorThumbnails);
        video.m_subCountText = obj.value("subCountText"_L1).toString();
        video.m_allowRatings = obj.value("allowRatings"_L1).toBool(true);
        video.m_rating = obj.value("rating"_L1).toDouble(5.0);
        video.m_isListed = obj.value("isListed"_L1).toBool(true);
        parseArray(obj.value("recommendedVideos"_L1), video.m_recommendedVideos);
        if (obj.contains(u"premiereTimestamp")) {
            video.m_premiereTimestamp = QDateTime::fromMSecsSinceEpoch(obj.value(u"premiereTimestamp").toInt());
        }
        video.m_hlsUrl = QUrl(obj.value(u"hlsUrl").toString());
        parseArray(obj.value(u"adaptiveFormats"), video.m_adaptiveFormats);
        parseArray(obj.value(u"formatStreams"), video.m_combinedFormats);
        parseArray(obj.value(u"captions"), video.m_captions);
    } else if (isPeerTube) {
        // TODO: peertube stub
    } else {
        video.m_author = obj.value("uploader"_L1).toString();
        parseArray(obj.value("tags"_L1), video.m_keywords);
        video.m_likeCount = obj.value("likes"_L1).toInt();
        video.m_dislikeCount = obj.value("dislikes"_L1).toInt();

        VideoThumbnail thumbnail;
        thumbnail.setUrl(QUrl(obj.value("uploaderAvatar"_L1).toString()));
        video.m_authorThumbnails.push_back(thumbnail);

        video.m_subCountText = QString::number(obj.value("uploaderSubscriberCount"_L1).toInt());
        parseArray(obj.value("relatedStreams"_L1), video.m_recommendedVideos);
        video.m_hlsUrl = QUrl(obj.value(u"hls").toString());
    }

    return video;
}

QStringList Video::keywords() const
{
    return m_keywords;
}

qint32 Video::likeCount() const
{
    return m_likeCount;
}

qint32 Video::dislikeCount() const
{
    return m_dislikeCount;
}

bool Video::isFamilyFriendly() const
{
    return m_isFamilyFriendly;
}

QStringList Video::allowedRegions() const
{
    return m_allowedRegions;
}

QString Video::genre() const
{
    return m_genre;
}

QString Video::genreUrl() const
{
    return m_genreUrl;
}

QList<VideoThumbnail> Video::authorThumbnails() const
{
    return m_authorThumbnails;
}

QString Video::subCountText() const
{
    return m_subCountText;
}

bool Video::allowRatings() const
{
    return m_allowRatings;
}

double Video::rating() const
{
    return m_rating;
}

bool Video::isListed() const
{
    return m_isListed;
}

QList<VideoBasicInfo> Video::recommendedVideos() const
{
    return m_recommendedVideos;
}

std::optional<QDateTime> Video::premiereTimestamp() const
{
    return m_premiereTimestamp;
}

QUrl Video::hlsUrl() const
{
    return m_hlsUrl;
}

QList<MediaFormat> Video::adaptiveFormats() const
{
    return m_adaptiveFormats;
}

QList<MediaFormatCombined> Video::combinedFormats() const
{
    return m_combinedFormats;
}

QList<Caption> Video::captions() const
{
    return m_captions;
}
