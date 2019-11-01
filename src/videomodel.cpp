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

#include "videomodel.h"
#include "videolistmodel.h"
#include "invidiousmanager.h"
#include <QNetworkReply>

VideoModel::VideoModel(QObject *parent)
    : QObject(parent), m_video(new VideoItem(this)),
      invidious(new InvidiousManager(this))
{
    connect(invidious, &InvidiousManager::videoReceived,
            this, &VideoModel::handleVideoReceived);
    connect(invidious, &InvidiousManager::videoRequestFailed,
            this, &VideoModel::handleRequestFailed);
}

void VideoModel::fetch()
{
    // if currently loading, abort
    if (m_isLoading) {
        lastRequest->abort();
        lastRequest->deleteLater();
        setIsLoading(false);
    }

    // clean up
    m_video->deleteLater();
    m_video = new VideoItem(this);
    emit videoChanged();

    setIsLoading(true);
    lastRequest = invidious->requestVideo(m_videoId);
}

void VideoModel::setIsLoading(bool loading)
{
    m_isLoading = loading;
    emit isLoadingChanged();
}

void VideoModel::handleVideoReceived(const QJsonObject &obj)
{
    setIsLoading(false);
    m_video->parseFromJson(obj);
    emit videoChanged();
}

void VideoModel::handleRequestFailed()
{
    setIsLoading(false);
}

VideoItem::VideoItem(QObject *parent)
    : QObject(parent)
{
}

QUrl VideoItem::thumbnailUrl(const QString &quality) const
{
    return thumbnail(quality).url();
}

QUrl VideoItem::authorThumbnail(qint32 size) const
{
    // thumbnails are sorted by size
    for (const VideoThumbnail &thumb : authorThumbnails()) {
        if (thumb.width() >= size)
            return thumb.url();
    }
    if (!authorThumbnails().isEmpty())
        return authorThumbnails().last().url();
    return {};
}

VideoListModel *VideoItem::recommendedVideosModel() const
{
    VideoListModel *videoModel = new VideoListModel(recommendedVideos(), (QObject*) this);
    return videoModel;
}
