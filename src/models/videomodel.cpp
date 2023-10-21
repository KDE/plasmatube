// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videomodel.h"
#include "controllers/plasmatube.h"
#include "videolistmodel.h"

#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QProcess>

using namespace Qt::StringLiterals;

VideoModel::VideoModel(QObject *parent)
    : QObject(parent)
    , m_video(new VideoItem(this))
{
    connect(this, &VideoModel::videoIdChanged, this, [this] {
        m_remoteUrl.clear();
        m_formatUrl.clear();
        Q_EMIT remoteUrlChanged();
    });
}

void VideoModel::fetch(const QString &videoId)
{
    m_videoId = videoId;

    // if currently loading, abort
    if (m_watcher) {
        m_watcher->cancel();
        m_watcher->deleteLater();
        m_watcher = nullptr;
    }

    // clean up
    m_video->deleteLater();
    m_video = new VideoItem(this);
    Q_EMIT videoChanged();

    auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestVideo(m_videoId);

    m_watcher = new QFutureWatcher<QInvidious::VideoResult>(this);
    connect(m_watcher, &QFutureWatcherBase::finished, this, [=] {
        auto result = m_watcher->result();

        if (const auto video = std::get_if<QInvidious::Video>(&result)) {
            m_video->deleteLater();
            m_video = new VideoItem(*video, this);
            Q_EMIT videoChanged();
        } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
            qDebug() << "VideoModel::fetch(): Error:" << error->second << error->first;
            Q_EMIT errorOccurred(error->second);
        }

        m_watcher->deleteLater();
        m_watcher = nullptr;
        Q_EMIT isLoadingChanged();
    });
    m_watcher->setFuture(future);
    Q_EMIT isLoadingChanged();

    // load format list
    QString youtubeDl = QStringLiteral("yt-dlp");
    QStringList arguments;
    arguments << QLatin1String("--dump-json") << m_videoId;
    QProcess *process = new QProcess();
    process->setReadChannel(QProcess::StandardOutput);
    process->start(youtubeDl, arguments);

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int, QProcess::ExitStatus) {
        const auto doc = QJsonDocument::fromJson(process->readAllStandardOutput());
        const auto formatsArray = doc.object()[QLatin1String("formats")].toArray();
        for (const auto &value : formatsArray) {
            const auto format = value.toObject();
            const auto formatNote = format["format_note"_L1].toString();
            if (formatNote == "medium"_L1) {
                m_audioUrl = format["url"_L1].toString();
            } else {
                m_formatUrl[formatNote] = format["url"_L1].toString();
            }
        }
        Q_EMIT remoteUrlChanged();
        Q_EMIT formatListChanged();
        process->deleteLater();
    });
}

bool VideoModel::isLoading() const
{
    return m_watcher != nullptr;
}

VideoItem::VideoItem(QObject *parent)
    : QObject(parent)
    , m_isLoaded(false)
{
}

VideoItem::VideoItem(const QInvidious::Video &video, QObject *parent)
    : QObject(parent)
    , m_isLoaded(true)
{
    *static_cast<QInvidious::Video *>(this) = video;
}

bool VideoItem::isLoaded() const
{
    return m_isLoaded;
}

QUrl VideoItem::thumbnailUrl(const QString &quality) const
{
    const QUrl thumbnailUrl = thumbnail(quality).url();

    if (!thumbnailUrl.isEmpty() && thumbnailUrl.isRelative()) {
        return QUrl(PlasmaTube::instance().sourceManager()->selectedSource()->api()->apiHost() + thumbnailUrl.toString(QUrl::FullyEncoded));
    }

    return thumbnailUrl;
}

QUrl VideoItem::authorThumbnail(quint32 size) const
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
    if (!m_formatUrl.isEmpty() && m_formatUrl.contains(m_selectedFormat)) {
        return m_formatUrl[m_selectedFormat];
    }
    return {};
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

VideoItem *VideoModel::video() const
{
    return m_video;
}

QString VideoModel::videoId() const
{
    return m_videoId;
}

void VideoModel::clearVideo()
{
    if (m_video) {
        m_video->deleteLater();
    }

    m_video = new VideoItem(this);
    Q_EMIT videoChanged();
}

#include "moc_videomodel.cpp"