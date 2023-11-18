// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDateTime>
#include <QUrl>

#include "qinvidiousglobal.h"
#include "videothumbnail.h"

namespace QInvidious
{

class VideoBasicInfo
{
public:
    FROM_JSON_OVERLOADS(VideoBasicInfo)
    static VideoBasicInfo fromJson(const QJsonObject &, VideoBasicInfo &);

    QString videoId() const;
    void setVideoId(const QString &);

    QString title() const;

    QList<VideoThumbnail> videoThumbnails() const;
    void setVideoThumbnails(const QList<VideoThumbnail> &);

    VideoThumbnail thumbnail(const QString &quality) const;

    QTime length() const;

    qint64 viewCount() const;

    QString author() const;

    QString authorId() const;

    QString authorUrl() const;

    QDateTime published() const;

    QString publishedText() const;

    QString description() const;

    QString descriptionHtml() const;

    bool liveNow() const;

    bool paid() const;

    bool premium() const;

    bool upcoming() const;

    QString indexId() const;

protected:
    QString m_videoId;
    QString m_title;
    QList<VideoThumbnail> m_videoThumbnails;
    QTime m_length;
    qint64 m_viewCount = 0;
    QString m_author;
    QString m_authorId;
    QString m_authorUrl;
    QDateTime m_published;
    QString m_publishedText;
    QString m_description;
    QString m_descriptionHtml;
    bool m_liveNow = false;
    bool m_paid = false;
    bool m_premium = false;
    bool m_upcoming = false;
    QString m_indexId;
};

}
