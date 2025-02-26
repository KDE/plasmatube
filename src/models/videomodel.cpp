// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videomodel.h"

#include "general_logging.h"
#include "plasmatube.h"
#include "videolistmodel.h"

#include <QFutureWatcher>
#include <QJsonArray>
#include <QProcess>

using namespace Qt::StringLiterals;

VideoModel::VideoModel(QObject *parent)
    : QObject(parent)
    , m_video(new VideoItem(this))
{
    connect(this, &VideoModel::videoIdChanged, this, [this] {
        m_remoteUrl.clear();
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
    connect(m_watcher, &QFutureWatcherBase::finished, this, [this] {
        const auto result = m_watcher->result();

        m_errorString.clear();

        if (const auto video = std::get_if<QInvidious::Video>(&result)) {
            m_video->deleteLater();
            m_video = new VideoItem(*video, this);

            if (!m_video->liveNow()) {
                // Formats that are separate video/audio streams
                for (const auto &value : m_video->adaptiveFormats()) {
                    if (value.mediaType() == QInvidious::MediaFormat::Audio) {
                        m_audioUrl = value.url().toString();
                        Q_EMIT audioUrlChanged();
                    } else {
                        m_formatUrl[value.qualityLabel()] = value.url().toString();
                    }
                }

                // Formats that are combined video/audio streams
                for (const auto &value : m_video->combinedFormats()) {
                    // Intentional, we only want to use combined formats as a last resort
                    if (!m_formatUrl.contains(value.qualityLabel())) {
                        m_formatUrl[value.qualityLabel()] = value.url().toString();
                    }
                }

                if (m_formatUrl.isEmpty() && !m_video->hlsUrl().isEmpty()) {
                    const auto preferredName =
                        videoQualityName(static_cast<Settings::EnumPreferredVideoQuality>(PlasmaTube::instance().settings()->preferredVideoQuality()));
                    m_formatUrl[preferredName] = m_video->hlsUrl().toString();
                }

                if (m_formatUrl.size() == 1) {
                    m_selectedFormat = m_formatUrl.keys().first();
                } else {
                    const auto preferredName =
                        videoQualityName(static_cast<Settings::EnumPreferredVideoQuality>(PlasmaTube::instance().settings()->preferredVideoQuality()));
                    if (m_formatUrl.contains(preferredName)) {
                        m_selectedFormat = preferredName;
                    } else {
                        // TODO: make sure it's lower resolution than the preferred one
                        m_selectedFormat = m_formatUrl.keys().last();
                    }
                }
            }

            Q_EMIT videoChanged();
            Q_EMIT formatListChanged();
            Q_EMIT remoteUrlChanged();
        } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
            qWarning(PLASMATUBE_GENERAL) << "VideoModel::fetch(): Error:" << error->second << error->first;
            Q_EMIT errorOccurred(error->second);
        }

        m_watcher->deleteLater();
        m_watcher = nullptr;
        Q_EMIT isLoadingChanged();
    });
    m_watcher->setFuture(future);
    Q_EMIT isLoadingChanged();
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
    if (!authorThumbs.isEmpty())
        return authorThumbs.last().url();
    return {};
}

VideoListModel *VideoItem::recommendedVideosModel()
{
    return new VideoListModel(recommendedVideos(), this);
}

QString VideoModel::remoteUrl()
{
    if (m_video->liveNow()) {
        return m_video->hlsUrl().toString();
    }
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
    QStringList keys = m_formatUrl.keys();
    std::sort(keys.begin(), keys.end(), [](const QString &a, const QString &b) -> bool {
        const auto parseQuality = [](const QString &quality) -> std::pair<int, int> {
            const QStringList parts = quality.split('p'_L1);
            // 1080p
            if (parts.length() == 2) {
                const int resolution = parts[0].toInt();
                return {resolution, 30}; // assume a framerate of 30
            } else if (parts.length() == 3) {
                // 1080p60
                const int resolution = parts[0].toInt();
                const int framerate = parts[2].toInt();
                return {resolution, framerate};
            } else {
                // we don't know?
                return {0, 0};
            }
        };

        const auto aQuality = parseQuality(a);
        const auto bQuality = parseQuality(b);

        // FIXME: 720p shows up above 720p60
        return aQuality.first > bQuality.first;
    });
    return keys;
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

QString VideoModel::getVideoUrl() const
{
    return PlasmaTube::instance().sourceManager()->selectedSource()->api()->getVideoUrl(m_videoId);
}

QString VideoModel::videoQualityName(const Settings::EnumPreferredVideoQuality videoQuality)
{
    switch (videoQuality) {
    case Settings::Q2160:
        return QStringLiteral("2160p");
    case Settings::Q1440:
        return QStringLiteral("1440p");
    case Settings::Q1080:
        return QStringLiteral("1080p");
    case Settings::Q720:
        return QStringLiteral("720p");
    case Settings::Q480:
        return QStringLiteral("480p");
    case Settings::Q360:
        return QStringLiteral("360p");
    case Settings::Q240:
        return QStringLiteral("240p");
    case Settings::Q144:
        return QStringLiteral("144p");
    }

    return QString{};
}

#include "moc_videomodel.cpp"
