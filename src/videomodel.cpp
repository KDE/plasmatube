// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videomodel.h"
#include "videolistmodel.h"
#include "invidiousmanager.h"
#include <QNetworkReply>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonArray>

VideoModel::VideoModel(QObject *parent)
    : QObject(parent), m_video(new VideoItem(this)),
      invidious(new InvidiousManager(this))
{
    connect(invidious, &InvidiousManager::videoReceived,
            this, &VideoModel::handleVideoReceived);
    connect(invidious, &InvidiousManager::videoRequestFailed,
            this, &VideoModel::handleRequestFailed);
    connect(this, &VideoModel::videoIdChanged,
            this, [this] {
                m_remoteUrl.clear();
                Q_EMIT remoteUrlChanged();
            });
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
    const auto authorThumbs = authorThumbnails();
    for (const auto &thumb : authorThumbs) {
        if (thumb.width() >= size)
            return thumb.url();
    }
    if (!authorThumbnails().isEmpty())
        return authorThumbnails().constEnd()->url();
    return {};
}

VideoListModel *VideoItem::recommendedVideosModel() const
{
    VideoListModel *videoModel = new VideoListModel(recommendedVideos(), (QObject*) this);
    return videoModel;
}

QString VideoModel::remoteUrl()
{
    if (!m_formatUrl.isEmpty()) {
        if (m_formatUrl.contains(m_selectedFormat)) {
            return m_formatUrl[m_selectedFormat];
        }
        return {};
    }

    QString youtubeDl = QStringLiteral("youtube-dl");
    QStringList arguments;
    arguments << QLatin1String("--dump-json")
              << m_videoId;
    QProcess *process = new QProcess();
    process->setReadChannel(QProcess::StandardOutput);
    process->start(youtubeDl, arguments);

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [=](int, QProcess::ExitStatus) {
                 const auto doc = QJsonDocument::fromJson(process->readAllStandardOutput());
                 const auto formatsArray = doc.object()[QLatin1String("formats")].toArray();
                 for (const auto &value : formatsArray) {
                    const auto format = value.toObject();
                    const auto formatNote = format["format_note"].toString();
                    if (formatNote == "tiny") {
                        m_audioUrl = format["url"].toString();
                    } else {
                        m_formatUrl[formatNote] = format["url"].toString();
                    }
                 }
                 Q_EMIT remoteUrlChanged();
                 process->deleteLater();
             });
    return QString();
}

QString VideoModel::audioUrl() const
{
    return m_audioUrl;
}

QStringList VideoModel::formatList() const
{
    return m_formatUrl.keys();
}

QString VideoModel::selectedFormat() const
{
    return m_selectedFormat;
}

void VideoModel::setSelectedFormat(const QString &selectedFormat)
{
    if (m_selectedFormat == selectedFormat) {
        return;
    }
    m_selectedFormat = selectedFormat;
    Q_EMIT remoteUrlChanged();
    Q_EMIT selectedFormatChanged();
}
