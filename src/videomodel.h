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

#ifndef VIDEOMODEL_H
#define VIDEOMODEL_H

#include <QAbstractItemModel>
#include "video.h"

class InvidiousManager;
class QNetworkReply;
class VideoListModel;

class VideoItem : public QObject, public Video
{
    Q_OBJECT
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
    VideoItem(QObject *parent = nullptr);

    Q_INVOKABLE QUrl thumbnailUrl(const QString &quality) const;
    Q_INVOKABLE QUrl authorThumbnail(qint32 size) const;
    Q_INVOKABLE VideoListModel *recommendedVideosModel() const;
};

class VideoModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLoading MEMBER m_isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString videoId MEMBER m_videoId NOTIFY videoIdChanged)
    Q_PROPERTY(VideoItem* video MEMBER m_video NOTIFY videoChanged)

public:
    explicit VideoModel(QObject *parent = nullptr);

    Q_INVOKABLE void fetch();
    void setIsLoading(bool);

signals:
    void isLoadingChanged();
    void videoIdChanged();
    void videoChanged();

private slots:
    void handleVideoReceived(const QJsonObject&);
    void handleRequestFailed();

private:
    bool m_isLoading = false;
    QString m_videoId;
    VideoItem* m_video;
    QNetworkReply *lastRequest = nullptr;
    InvidiousManager *invidious;
};

#endif // VIDEOMODEL_H
