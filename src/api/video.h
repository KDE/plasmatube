// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "caption.h"
#include "mediaformat.h"
#include "videobasicinfo.h"

#include <optional>

namespace QInvidious
{

class Video : public VideoBasicInfo
{
public:
    FROM_JSON_OVERLOADS(Video)
    static Video fromJson(const QJsonObject &, Video &);

    QStringList keywords() const;

    qint32 likeCount() const;

    qint32 dislikeCount() const;

    bool isFamilyFriendly() const;

    QStringList allowedRegions() const;

    QString genre() const;

    QString genreUrl() const;

    QList<VideoThumbnail> authorThumbnails() const;

    QString subCountText() const;

    bool allowRatings() const;

    double rating() const;

    bool isListed() const;

    QList<VideoBasicInfo> recommendedVideos() const;

    std::optional<QDateTime> premiereTimestamp() const;

    QUrl hlsUrl() const;

    QList<MediaFormat> adaptiveFormats() const;

    QList<MediaFormatCombined> combinedFormats() const;

    QList<Caption> captions() const;

private:
    QStringList m_keywords;
    QString m_viewCountText;
    qint32 m_likeCount = 0;
    qint32 m_dislikeCount = 0;
    QStringList m_allowedRegions;
    QString m_genre;
    QString m_genreUrl;
    QList<VideoThumbnail> m_authorThumbnails;
    QString m_subCountText;
    double m_rating = 1.0;
    QList<VideoBasicInfo> m_recommendedVideos;
    std::optional<QDateTime> m_premiereTimestamp;
    QUrl m_hlsUrl;
    QList<MediaFormat> m_adaptiveFormats;
    QList<MediaFormatCombined> m_combinedFormats;
    QList<Caption> m_captions;
    bool m_isFamilyFriendly = false;
    bool m_allowRatings = false;
    bool m_isListed = false;
};

}
