// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "video.h"
#include <QJsonArray>

using namespace QInvidious;

Video Video::fromJson(const QJsonObject &obj, Video &video)
{
    VideoBasicInfo::fromJson(obj, video);

    auto jsonValueToString = [](const QJsonValue &val) {
        return val.toString();
    };

    const auto keywords = obj.value("keywords").toArray();
    std::transform(keywords.cbegin(), keywords.cend(), std::back_inserter(video.m_keywords), jsonValueToString);

    video.setLikeCount(obj.value("likeCount").toInt());
    video.setDislikeCount(obj.value("dislikeCount").toInt());
    video.setIsFamilyFriendly(obj.value("isFamilyFriendly").toBool(true));

    const auto allowedRegions = obj.value("allowedRegions").toArray();
    std::transform(allowedRegions.cbegin(), allowedRegions.cend(), std::back_inserter(video.m_allowedRegions), jsonValueToString);

    video.setGenre(obj.value("genre").toString());
    video.setGenreUrl(obj.value("genreUrl").toString());

    const auto authorThumbnails = obj.value("authorThumbnails").toArray();
    std::transform(authorThumbnails.cbegin(), authorThumbnails.cend(), std::back_inserter(video.m_authorThumbnails),
                   [](const QJsonValue &val) {
        return VideoThumbnail::fromJson(val);
    });

    video.setSubCountText(obj.value("subCountText").toString());
    video.setAllowRatings(obj.value("allowRatings").toBool(true));
    video.setRating(obj.value("rating").toDouble(5.0));
    video.setIsListed(obj.value("isListed").toBool(true));

    const auto recommendedVideos = obj.value("recommendedVideos").toArray();
    std::transform(recommendedVideos.cbegin(), recommendedVideos.cend(), std::back_inserter(video.m_recommendedVideos),
                   [](const QJsonValue &val) {
        return VideoBasicInfo::fromJson(val);
    });

    return video;
}

Video::Video()
    : m_likeCount(0),
      m_dislikeCount(0),
      m_isFamilyFriendly(true),
      m_allowRatings(true),
      m_rating(1.0),
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
