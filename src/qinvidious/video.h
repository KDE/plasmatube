// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VIDEO_H
#define VIDEO_H

#include "videobasicinfo.h"
#include "mediaformat.h"
#include "caption.h"

#include <optional>

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

    std::optional<QDateTime> premiereTimestamp() const;
    void setPremiereTimestamp(const std::optional<QDateTime> &premiereTimestamp);

    QUrl hlsUrl() const;
    void setHlsUrl(const QUrl &hlsUrl);

    QList<MediaFormat> adaptiveFormats() const;
    void setAdaptiveFormats(const QList<MediaFormat> &adaptiveFormats);

    QList<MediaFormatCombined> combinedFormats() const;
    void setCombinedFormats(const QList<MediaFormatCombined> &combinedFormats);

    QList<Caption> captions() const;
    void setCaptions(const QList<Caption> &captions);

private:
    QStringList m_keywords;
    QString m_viewCountText;
    qint32 m_likeCount;
    qint32 m_dislikeCount;
    QStringList m_allowedRegions;
    QString m_genre;
    QString m_genreUrl;
    QList<VideoThumbnail> m_authorThumbnails;
    QString m_subCountText;
    double m_rating;
    QList<VideoBasicInfo> m_recommendedVideos;
    std::optional<QDateTime> m_premiereTimestamp;
    QUrl m_hlsUrl;
    QList<MediaFormat> m_adaptiveFormats;
    QList<MediaFormatCombined> m_combinedFormats;
    QList<Caption> m_captions;
    bool m_isFamilyFriendly;
    bool m_allowRatings;
    bool m_isListed;
};

};

#endif // VIDEO_H
