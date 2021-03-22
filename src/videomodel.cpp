// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videomodel.h"
#include "videolistmodel.h"
#include "plasmatube.h"

#include <QFutureWatcher>
#include <QNetworkReply>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonArray>

VideoModel::VideoModel(QObject *parent)
    : QObject(parent),
      m_video(new VideoItem(this))
{
    connect(this, &VideoModel::videoIdChanged, this, [this] {
        m_remoteUrl.clear();
        Q_EMIT remoteUrlChanged();
    });
}

void VideoModel::fetch()
{
    // if currently loading, abort
    if (m_watcher) {
        m_watcher->cancel();
        m_watcher->deleteLater();
        m_watcher = nullptr;
    }

    // clean up
    m_video->deleteLater();
    m_video = new VideoItem(this);
    emit videoChanged();

    auto future = PlasmaTube::instance().api()->requestVideo(m_videoId);

    m_watcher = new QFutureWatcher<QInvidious::VideoResult>(this);
    connect(m_watcher, &QFutureWatcherBase::finished, this, [=] {
        auto result = m_watcher->result();

        if (const auto video = std::get_if<QInvidious::Video>(&result)) {
            m_video->deleteLater();
            m_video = new VideoItem(*video, this);
            emit videoChanged();
        } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
            emit errorOccurred(error->second);
        }

        m_watcher->deleteLater();
        m_watcher = nullptr;
        emit isLoadingChanged();
    });
    m_watcher->setFuture(future);
    emit isLoadingChanged();
}

bool VideoModel::isLoading() const
{
    return m_watcher != nullptr;
}

VideoItem::VideoItem(QObject *parent)
    : QObject(parent)
{
}

VideoItem::VideoItem(const QInvidious::Video &video, QObject *parent)
    : QObject(parent)
{
    *static_cast<QInvidious::Video *>(this) = video;
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

VideoListModel *VideoItem::recommendedVideosModel()
{
    return new VideoListModel(recommendedVideos(), this);
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
