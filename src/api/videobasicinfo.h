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

    VideoBasicInfo();

    bool isNotification() const;
    void setIsNotification(bool);

    QString videoId() const;
    void setVideoId(const QString &);

    QString title() const;
    void setTitle(const QString &);

    QList<VideoThumbnail> videoThumbnails() const;
    void setVideoThumbnails(const QList<VideoThumbnail> &);

    VideoThumbnail thumbnail(const QString &quality) const;

    QTime length() const;
    void setLength(const QTime &);

    qint64 viewCount() const;
    void setViewCount(qint64);

    QString author() const;
    void setAuthor(const QString &);

    QString authorId() const;
    void setAuthorId(const QString &);

    QString authorUrl() const;
    void setAuthorUrl(const QString &);

    QDateTime published() const;
    void setPublished(const QDateTime &);

    QString publishedText() const;
    void setPublishedText(const QString &);

    QString description() const;
    void setDescription(const QString &);

    QString descriptionHtml() const;
    void setDescriptionHtml(const QString &);

    bool liveNow() const;
    void setLiveNow(bool);

    bool paid() const;
    void setPaid(bool);

    bool premium() const;
    void setPremium(bool);

    bool upcoming() const;
    void setUpcoming(bool upcoming);

    QString indexId() const;
    void setIndexId(const QString &);

private:
    QString m_videoId;
    QString m_title;
    QList<VideoThumbnail> m_videoThumbnails;
    QTime m_length;
    qint64 m_viewCount;
    QString m_author;
    QString m_authorId;
    QString m_authorUrl;
    QDateTime m_published;
    QString m_publishedText;
    QString m_description;
    QString m_descriptionHtml;
    bool m_isNotification;
    bool m_liveNow;
    bool m_paid;
    bool m_premium;
    bool m_upcoming;
    QString m_indexId;
};

};
