// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "video.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

Video Video::fromJson(const QJsonObject &obj, Video &video)
{
    VideoBasicInfo::fromJson(obj, video);

    parseArray(obj.value("keywords"_L1), video.m_keywords);
    video.setLikeCount(obj.value("likeCount"_L1).toInt());
    video.setDislikeCount(obj.value("dislikeCount"_L1).toInt());
    video.setIsFamilyFriendly(obj.value("isFamilyFriendly"_L1).toBool(true));
    parseArray(obj.value("allowedRegions"_L1), video.m_allowedRegions);
    video.setGenre(obj.value("genre"_L1).toString());
    video.setGenreUrl(obj.value("genreUrl"_L1).toString());
    parseArray(obj.value("authorThumbnails"_L1), video.m_authorThumbnails);
    video.setSubCountText(obj.value("subCountText"_L1).toString());
    video.setAllowRatings(obj.value("allowRatings"_L1).toBool(true));
    video.setRating(obj.value("rating"_L1).toDouble(5.0));
    video.setIsListed(obj.value("isListed"_L1).toBool(true));
    parseArray(obj.value("recommendedVideos"_L1), video.m_recommendedVideos);
    if (obj.contains(u"premiereTimestamp")) {
        video.setPremiereTimestamp(
            QDateTime::fromMSecsSinceEpoch(
                obj.value(u"premiereTimestamp").toInt()
            )
        );
    }
    video.setHlsUrl(QUrl(obj.value(u"hlsUrl").toString()));
    parseArray(obj.value(u"adaptiveFormats"), video.m_adaptiveFormats);
    parseArray(obj.value(u"formatStreams"), video.m_combinedFormats);
    parseArray(obj.value(u"captions"), video.m_captions);
    return video;
}

Video::Video()
    : m_likeCount(0),
      m_dislikeCount(0),
      m_rating(1.0),
      m_isFamilyFriendly(true),
      m_allowRatings(true),
      m_isListed(true)
{
}

QStringList Video::keywords() const
{
    return m_keywords;
}

void Video::setKeywords(const QStringList &keywords)
{
    m_keywords = keywords;
}

qint32 Video::likeCount() const
{
    return m_likeCount;
}

void Video::setLikeCount(qint32 likeCount)
{
    m_likeCount = likeCount;
}

qint32 Video::dislikeCount() const
{
    return m_dislikeCount;
}

void Video::setDislikeCount(qint32 dislikeCount)
{
    m_dislikeCount = dislikeCount;
}

bool Video::isFamilyFriendly() const
{
    return m_isFamilyFriendly;
}

void Video::setIsFamilyFriendly(bool isFamilyFriendly)
{
    m_isFamilyFriendly = isFamilyFriendly;
}

QStringList Video::allowedRegions() const
{
    return m_allowedRegions;
}

void Video::setAllowedRegions(const QStringList &allowedRegions)
{
    m_allowedRegions = allowedRegions;
}

QString Video::genre() const
{
    return m_genre;
}

void Video::setGenre(const QString &genre)
{
    m_genre = genre;
}

QString Video::genreUrl() const
{
    return m_genreUrl;
}

void Video::setGenreUrl(const QString &genreUrl)
{
    m_genreUrl = genreUrl;
}

QList<VideoThumbnail> Video::authorThumbnails() const
{
    return m_authorThumbnails;
}

void Video::setAuthorThumbnails(const QList<VideoThumbnail> &authorThumbnails)
{
    m_authorThumbnails = authorThumbnails;
}

QString Video::subCountText() const
{
    return m_subCountText;
}

void Video::setSubCountText(const QString &subCountText)
{
    m_subCountText = subCountText;
}

bool Video::allowRatings() const
{
    return m_allowRatings;
}

void Video::setAllowRatings(bool allowRatings)
{
    m_allowRatings = allowRatings;
}

double Video::rating() const
{
    return m_rating;
}

void Video::setRating(double rating)
{
    m_rating = rating;
}

bool Video::isListed() const
{
    return m_isListed;
}

void Video::setIsListed(bool isListed)
{
    m_isListed = isListed;
}

QList<VideoBasicInfo> Video::recommendedVideos() const
{
    return m_recommendedVideos;
}

void Video::setRecommendedVideos(const QList<VideoBasicInfo> &recommendedVideos)
{
    m_recommendedVideos = recommendedVideos;
}

std::optional<QDateTime> Video::premiereTimestamp() const
{
    return m_premiereTimestamp;
}

void Video::setPremiereTimestamp(const std::optional<QDateTime> &premiereTimestamp)
{
    m_premiereTimestamp = premiereTimestamp;
}

QUrl Video::hlsUrl() const
{
    return m_hlsUrl;
}

void Video::setHlsUrl(const QUrl &hlsUrl)
{
    m_hlsUrl = hlsUrl;
}

QList<MediaFormat> Video::adaptiveFormats() const
{
    return m_adaptiveFormats;
}

void Video::setAdaptiveFormats(const QList<MediaFormat> &adaptiveFormats)
{
    m_adaptiveFormats = adaptiveFormats;
}

QList<MediaFormatCombined> Video::combinedFormats() const
{
    return m_combinedFormats;
}

void Video::setCombinedFormats(const QList<MediaFormatCombined> &combinedFormats)
{
    m_combinedFormats = combinedFormats;
}

QList<Caption> Video::captions() const
{
    return m_captions;
}

void Video::setCaptions(const QList<Caption> &captions)
{
    m_captions = captions;
}
