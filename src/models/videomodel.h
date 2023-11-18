// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractapi.h"

#include <video.h>

#include <QtQml>

template<typename T>
class QFutureWatcher;
class VideoListModel;

class VideoItem : public QObject, public QInvidious::Video
{
    Q_OBJECT

    Q_PROPERTY(bool isLoaded READ isLoaded CONSTANT)
    Q_PROPERTY(QString videoId READ videoId CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QTime length READ length CONSTANT)
    Q_PROPERTY(qint64 viewCount READ viewCount CONSTANT)
    Q_PROPERTY(QString author READ author CONSTANT)
    Q_PROPERTY(QString authorId READ authorId CONSTANT)
    Q_PROPERTY(QString authorUrl READ authorUrl CONSTANT)
    Q_PROPERTY(QDateTime published READ published CONSTANT)
    Q_PROPERTY(QString publishedText READ publishedText CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString descriptionHtml READ descriptionHtml CONSTANT)
    Q_PROPERTY(bool liveNow READ liveNow CONSTANT)
    Q_PROPERTY(bool paid READ paid CONSTANT)
    Q_PROPERTY(bool premium READ premium CONSTANT)
    Q_PROPERTY(QStringList keywords READ keywords CONSTANT)
    Q_PROPERTY(qint32 likeCount READ likeCount CONSTANT)
    Q_PROPERTY(qint32 dislikeCount READ dislikeCount CONSTANT)
    Q_PROPERTY(bool isFamilyFriendly READ isFamilyFriendly CONSTANT)
    Q_PROPERTY(QStringList allowedRegions READ allowedRegions CONSTANT)
    Q_PROPERTY(QString genre READ genre CONSTANT)
    Q_PROPERTY(QString genreUrl READ genreUrl CONSTANT)
    Q_PROPERTY(QString subCountText READ subCountText CONSTANT)
    Q_PROPERTY(bool allowRatings READ allowRatings CONSTANT)
    Q_PROPERTY(double rating READ rating CONSTANT)
    Q_PROPERTY(bool isListed READ isListed CONSTANT)

public:
    explicit VideoItem(QObject *parent = nullptr);
    explicit VideoItem(const QInvidious::Video &, QObject *parent = nullptr);

    bool isLoaded() const;
    Q_INVOKABLE QUrl thumbnailUrl(const QString &quality) const;
    Q_INVOKABLE QUrl authorThumbnail(quint32 size) const;
    Q_INVOKABLE VideoListModel *recommendedVideosModel();

private:
    bool m_isLoaded = false;
};

class VideoModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString videoId MEMBER m_videoId NOTIFY videoIdChanged)
    Q_PROPERTY(QString remoteUrl READ remoteUrl NOTIFY remoteUrlChanged)
    Q_PROPERTY(QString audioUrl READ audioUrl NOTIFY remoteUrlChanged)
    Q_PROPERTY(VideoItem *video MEMBER m_video NOTIFY videoChanged)
    Q_PROPERTY(QStringList formatList READ formatList NOTIFY formatListChanged)
    Q_PROPERTY(QString selectedFormat READ selectedFormat WRITE setSelectedFormat NOTIFY selectedFormatChanged)

public:
    explicit VideoModel(QObject *parent = nullptr);

    void fetch(const QString &videoId);

    bool isLoading() const;
    QString remoteUrl();
    QString audioUrl() const;
    QStringList formatList() const;
    QString selectedFormat() const;
    void setSelectedFormat(const QString &selectedFormat);

    VideoItem *video() const;
    QString videoId() const;
    void clearVideo();

Q_SIGNALS:
    void isLoadingChanged();
    void videoIdChanged();
    void videoChanged();
    void remoteUrlChanged();
    void formatListChanged();
    void selectedFormatChanged();
    void errorOccurred(const QString &errorText);

private:
    QString m_videoId;
    QString m_remoteUrl;
    QHash<QString, QString> m_formatUrl;
    QString m_selectedFormat = QStringLiteral("720p");
    QString m_audioUrl;
    VideoItem *m_video = nullptr;
    QFutureWatcher<QInvidious::VideoResult> *m_watcher = nullptr;
};