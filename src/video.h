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

#ifndef VIDEO_H
#define VIDEO_H

#include "videobasicinfo.h"

class Video : public VideoBasicInfo
{
public:
    void parseFromJson(const QJsonObject&);

public:
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

#endif // VIDEO_H
