// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VIDEOBASICINFO_H
#define VIDEOBASICINFO_H

#include <QDateTime>
#include <QString>
#include <QUrl>
#include <QJsonObject>

class VideoThumbnail
{
public:
    void parseFromJson(const QJsonObject&);

    QString quality() const;
    void setQuality(const QString&);

    QUrl url() const;
    void setUrl(const QUrl&);

    qint32 width() const;
    void setWidth(qint32);

    qint32 height() const;
    void setHeight(qint32);

private:
    QString m_quality;
    QUrl m_url;
    qint32 m_width;
    qint32 m_height;
};

class VideoBasicInfo
{
public:
    void parseFromJson(const QJsonObject&);

    bool isNotification() const;
    void setIsNotification(bool);

    QString videoId() const;
    void setVideoId(const QString&);

    QString title() const;
    void setTitle(const QString&);

    QList<VideoThumbnail> videoThumbnails() const;
    void setVideoThumbnails(const QList<VideoThumbnail>&);

    VideoThumbnail thumbnail(const QString &quality) const;

    QTime length() const;
    void setLength(const QTime&);

    qint64 viewCount() const;
    void setViewCount(qint64);

    QString author() const;
    void setAuthor(const QString&);

    QString authorId() const;
    void setAuthorId(const QString&);

    QString authorUrl() const;
    void setAuthorUrl(const QString&);

    QDateTime published() const;
    void setPublished(const QDateTime&);

    QString publishedText() const;
    void setPublishedText(const QString&);

    QString description() const;
    void setDescription(const QString&);

    QString descriptionHtml() const;
    void setDescriptionHtml(const QString&);

    bool liveNow() const;
    void setLiveNow(bool);

    bool paid() const;
    void setPaid(bool);

    bool premium() const;
    void setPremium(bool);

private:
    bool m_isNotification = false;
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
    bool m_liveNow = false;
    bool m_paid = false;
    bool m_premium = false;
};

#endif // VIDEOBASICINFO_H
