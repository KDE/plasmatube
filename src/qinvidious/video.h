// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VIDEO_H
#define VIDEO_H

#include "videobasicinfo.h"

namespace QInvidious {

class Video : public VideoBasicInfo
{
public:
    FROM_JSON_OVERLOADS(Video)
    static Video fromJson(const QJsonObject &, Video &);

    Video();

    QStringList keywords() const;
    void setKeywords(const QStringList&);

    qint32 likeCount() const;
    void setLikeCount(qint32);

    qint32 dislikeCount() const;
    void setDislikeCount(qint32);

    bool isFamilyFriendly() const;
    void setIsFamilyFriendly(bool);

    QStringList allowedRegions() const;
    void setAllowedRegions(const QStringList&);

    QString genre() const;
    void setGenre(const QString&);

    QString genreUrl() const;
    void setGenreUrl(const QString&);

    QList<VideoThumbnail> authorThumbnails() const;
    void setAuthorThumbnails(const QList<VideoThumbnail>&);

    QString subCountText() const;
    void setSubCountText(const QString&);

    bool allowRatings() const;
    void setAllowRatings(bool);

    double rating() const;
    void setRating(double);

    bool isListed() const;
    void setIsListed(bool);

    QList<VideoBasicInfo> recommendedVideos() const;
    void setRecommendedVideos(const QList<VideoBasicInfo>&);

private:
    QStringList m_keywords;
    qint32 m_likeCount;
    qint32 m_dislikeCount;
    bool m_isFamilyFriendly;
    QStringList m_allowedRegions;
    QString m_genre;
    QString m_genreUrl;
    QList<VideoThumbnail> m_authorThumbnails;
    QString m_subCountText;
    bool m_allowRatings;
    double m_rating;
    bool m_isListed;
    QList<VideoBasicInfo> m_recommendedVideos;
};

};

#endif // VIDEO_H
