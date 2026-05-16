// SPDX-FileCopyrightText: 2026 Plasmatube Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "youtubeapi.h"

#include "channel.h"
#include "searchparameters.h"
#include "searchresult.h"
#include "video.h"

#include <KLocalizedString>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QRegularExpression>
#include <QStringBuilder>
#include <QUrlQuery>

using namespace QInvidious;
using namespace Qt::StringLiterals;

namespace
{
constexpr auto INNERTUBE_HOST = "https://www.youtube.com";
constexpr auto INNERTUBE_API_KEY = "AIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8";
constexpr auto CLIENT_NAME = "TVHTML5";
constexpr auto CLIENT_VERSION = "7.20260311.12.00";
constexpr auto USER_AGENT = "Mozilla/5.0 (ChromiumStylePlatform) Cobalt/Version";
}

YouTubeApi::YouTubeApi(QNetworkAccessManager *netManager, QObject *parent)
    : AbstractApi(netManager, parent)
{
}

bool YouTubeApi::isLoggedIn() const
{
    return false;
}

bool YouTubeApi::canLogIn() const
{
    return false;
}

void YouTubeApi::loadCredentials(const QString &prefix)
{
    Q_UNUSED(prefix)
}

void YouTubeApi::saveCredentials(const QString &prefix)
{
    Q_UNUSED(prefix)
}

void YouTubeApi::wipeCredentials(const QString &prefix)
{
    Q_UNUSED(prefix)
}

bool YouTubeApi::supportsFeature(AbstractApi::SupportedFeature feature)
{
    switch (feature) {
    case SponsorBlock:
    case RelatedVideos:
        return true;
    case PopularPage:
    case TrendingCategories:
        return false;
    }
    return false;
}

QFuture<LogInResult> YouTubeApi::logIn(const QString &username, const QString &password)
{
    Q_UNUSED(username)
    Q_UNUSED(password)
    return readyResult<LogInResult>(notImplementedError());
}

QFuture<VideoResult> YouTubeApi::requestVideo(const QString &videoId)
{
    QJsonObject payload{{u"videoId"_s, videoId}};
    return innertubePost<VideoResult>(u"next"_s, payload, [this, videoId](QNetworkReply *reply) -> VideoResult {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            return invalidJsonError();
        }
        const auto root = doc.object();

        QJsonObject synthesized;
        synthesized.insert(u"videoId"_s, videoId);

        std::function<const QJsonObject(const QJsonValue &, const QString &)> findFirst;
        findFirst = [&findFirst](const QJsonValue &v, const QString &key) -> const QJsonObject {
            if (v.isObject()) {
                const auto obj = v.toObject();
                if (obj.contains(key)) {
                    return obj.value(key).toObject();
                }
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    auto r = findFirst(it.value(), key);
                    if (!r.isEmpty()) {
                        return r;
                    }
                }
            } else if (v.isArray()) {
                for (const auto &x : v.toArray()) {
                    auto r = findFirst(x, key);
                    if (!r.isEmpty()) {
                        return r;
                    }
                }
            }
            return {};
        };

        const auto meta = findFirst(root, u"videoMetadataRenderer"_s);
        synthesized.insert(u"title"_s, flattenRuns(meta.value(u"title"_s)));
        const auto owner = meta.value(u"owner"_s).toObject().value(u"videoOwnerRenderer"_s).toObject();
        synthesized.insert(u"author"_s, flattenRuns(owner.value(u"title"_s)));
        synthesized.insert(u"authorId"_s,
                           owner.value(u"navigationEndpoint"_s).toObject().value(u"browseEndpoint"_s).toObject().value(u"browseId"_s).toString());

        const auto viewCounts = findFirst(root, u"videoViewCountRenderer"_s);
        synthesized.insert(u"viewCount"_s, parseDigits(flattenRuns(viewCounts.value(u"viewCount"_s))));

        const auto description = findFirst(root, u"expandableVideoDescriptionBodyRenderer"_s);
        const auto descText = flattenRuns(description.value(u"descriptionBodyText"_s));
        synthesized.insert(u"description"_s, descText);
        synthesized.insert(u"descriptionHtml"_s, descText);

        const auto info = dumpVideoInfo(videoId);
        if (info.isEmpty()) {
            return Error{QNetworkReply::ContentNotFoundError, i18n("yt-dlp could not extract this video. YouTube may be requiring sign-in.")};
        }

        QJsonArray adaptive, combined;
        splitFormats(info.value(u"formats"_s).toArray(), adaptive, combined);
        if (adaptive.isEmpty() && combined.isEmpty()) {
            return Error{QNetworkReply::ContentNotFoundError, i18n("No playable formats returned.")};
        }
        synthesized.insert(u"adaptiveFormats"_s, adaptive);
        synthesized.insert(u"formatStreams"_s, combined);

        return Video::fromJson(synthesized);
    });
}

QFuture<SearchListResult> YouTubeApi::requestSearchResults(const SearchParameters &parameters, Paginator *paginator)
{
    Q_UNUSED(paginator)

    QJsonObject payload{{u"query"_s, parameters.query()}};
    return innertubePost<SearchListResult>(u"search"_s, payload, [](QNetworkReply *reply) -> SearchListResult {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            return invalidJsonError();
        }

        QList<SearchResult> results;
        for (const auto &item : walkVideoItems(doc.object())) {
            results << SearchResult::fromJson(item);
        }
        return results;
    });
}

QFuture<VideoListResult> YouTubeApi::requestFeed(Paginator *paginator)
{
    Q_UNUSED(paginator)
    return readyResult<VideoListResult>(notImplementedError());
}

QFuture<VideoListResult> YouTubeApi::requestTrending(TrendingTopic topic, Paginator *paginator)
{
    Q_UNUSED(topic)
    Q_UNUSED(paginator)

    QJsonObject payload{{u"browseId"_s, u"FEwhat_to_watch"_s}};
    return innertubePost<VideoListResult>(u"browse"_s, payload, [](QNetworkReply *reply) -> VideoListResult {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            return invalidJsonError();
        }
        return parseVideoRenderers(doc.object());
    });
}

QFuture<VideoListResult> YouTubeApi::requestChannel(const QString &query, Paginator *paginator)
{
    Q_UNUSED(paginator)

    QJsonObject payload{{u"browseId"_s, query}, {u"params"_s, u"EgZ2aWRlb3PyBgQKAjoA"_s}};
    return innertubePost<VideoListResult>(u"browse"_s, payload, [](QNetworkReply *reply) -> VideoListResult {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            return invalidJsonError();
        }
        return parseVideoRenderers(doc.object());
    });
}

QFuture<SubscriptionsResult> YouTubeApi::requestSubscriptions()
{
    return readyResult<SubscriptionsResult>(notImplementedError());
}

QFuture<Result> YouTubeApi::subscribeToChannel(const QString &channel)
{
    Q_UNUSED(channel)
    return readyResult<Result>(notImplementedError());
}

QFuture<Result> YouTubeApi::unsubscribeFromChannel(const QString &channel)
{
    Q_UNUSED(channel)
    return readyResult<Result>(notImplementedError());
}

QFuture<HistoryResult> YouTubeApi::requestHistory(Paginator *paginator)
{
    Q_UNUSED(paginator)
    return readyResult<HistoryResult>(notImplementedError());
}

QFuture<Result> YouTubeApi::markWatched(const QString &videoId)
{
    Q_UNUSED(videoId)
    return readyResult<Result>(notImplementedError());
}

QFuture<Result> YouTubeApi::markUnwatched(const QString &videoId)
{
    Q_UNUSED(videoId)
    return readyResult<Result>(notImplementedError());
}

QFuture<CommentsResult> YouTubeApi::requestComments(const QString &videoId, Paginator *paginator)
{
    Q_UNUSED(videoId)
    Q_UNUSED(paginator)
    return readyResult<CommentsResult>(notImplementedError());
}

QFuture<PlaylistsResult> YouTubeApi::requestPlaylists()
{
    return readyResult<PlaylistsResult>(notImplementedError());
}

QFuture<PreferencesResult> YouTubeApi::requestPreferences()
{
    return readyResult<PreferencesResult>(notImplementedError());
}

QFuture<Result> YouTubeApi::setPreferences(const QInvidious::Preferences &preferences)
{
    Q_UNUSED(preferences)
    return readyResult<Result>(notImplementedError());
}

QFuture<VideoListResult> YouTubeApi::requestPlaylist(const QString &plid)
{
    Q_UNUSED(plid)
    return readyResult<VideoListResult>(notImplementedError());
}

QFuture<ChannelResult> YouTubeApi::requestChannelInfo(const QString &channelId)
{
    QJsonObject payload{{u"browseId"_s, channelId}};
    return innertubePost<ChannelResult>(u"browse"_s, payload, [channelId](QNetworkReply *reply) -> ChannelResult {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            return invalidJsonError();
        }

        const auto header = doc.object().value(u"header"_s).toObject();
        const auto tabbed = !header.value(u"c4TabbedHeaderRenderer"_s).toObject().isEmpty() ? header.value(u"c4TabbedHeaderRenderer"_s).toObject()
                                                                                            : header.value(u"pageHeaderRenderer"_s).toObject();

        QJsonObject synthesized;
        synthesized.insert(u"authorId"_s, channelId);
        synthesized.insert(u"author"_s,
                           tabbed.value(u"title"_s).toString().isEmpty() ? flattenRuns(tabbed.value(u"title"_s)) : tabbed.value(u"title"_s).toString());
        synthesized.insert(u"description"_s, flattenRuns(tabbed.value(u"tagline"_s)));
        synthesized.insert(u"subCountText"_s, flattenRuns(tabbed.value(u"subscriberCountText"_s)));

        QJsonArray avatars;
        for (const auto &t : tabbed.value(u"avatar"_s).toObject().value(u"thumbnails"_s).toArray()) {
            avatars.append(t);
        }
        synthesized.insert(u"authorThumbnails"_s, avatars);

        QJsonArray banners;
        for (const auto &t : tabbed.value(u"banner"_s).toObject().value(u"thumbnails"_s).toArray()) {
            banners.append(t);
        }
        synthesized.insert(u"authorBanners"_s, banners);

        return Channel::fromJson(synthesized);
    });
}

QFuture<PlaylistsResult> YouTubeApi::requestChannelPlaylists(const QString &channelId)
{
    Q_UNUSED(channelId)
    return readyResult<PlaylistsResult>(notImplementedError());
}

QFuture<Result> YouTubeApi::addVideoToPlaylist(const QString &plid, const QString &videoId)
{
    Q_UNUSED(plid)
    Q_UNUSED(videoId)
    return readyResult<Result>(notImplementedError());
}

QFuture<Result> YouTubeApi::removeVideoFromPlaylist(const QString &plid, const QString &indexId)
{
    Q_UNUSED(plid)
    Q_UNUSED(indexId)
    return readyResult<Result>(notImplementedError());
}

QString YouTubeApi::getVideoUrl(const QString &videoId)
{
    return QStringLiteral("https://youtube.com/watch?v=%1").arg(videoId);
}

void YouTubeApi::setCookiesFromBrowser(const QString &browser, const QString &profile)
{
    if (m_cookiesFromBrowser != browser || m_cookiesBrowserProfile != profile) {
        m_cookiesFromBrowser = browser;
        m_cookiesBrowserProfile = profile;
        m_videoInfoCache.clear();
    }
}

QJsonObject YouTubeApi::dumpVideoInfo(const QString &videoId)
{
    if (auto it = m_videoInfoCache.constFind(videoId); it != m_videoInfoCache.cend()) {
        return *it;
    }

    QStringList args{QStringLiteral("-J"), QStringLiteral("--no-playlist"), QStringLiteral("--no-warnings")};
    if (!m_cookiesFromBrowser.isEmpty()) {
        QString spec = m_cookiesFromBrowser;
        if (!m_cookiesBrowserProfile.isEmpty()) {
            spec += u':' + m_cookiesBrowserProfile;
        }
        args << QStringLiteral("--cookies-from-browser") << spec;
    }
    args << QStringLiteral("https://www.youtube.com/watch?v=%1").arg(videoId);

    qDebug() << "yt-dlp" << args;
    QProcess proc;
    proc.start(QStringLiteral("yt-dlp"), args);
    if (!proc.waitForStarted(2000)) {
        qWarning() << "yt-dlp not found on PATH";
        return {};
    }
    if (!proc.waitForFinished(60000) || proc.exitCode() != 0) {
        qWarning() << "yt-dlp failed for" << videoId << proc.readAllStandardError();
        return {};
    }
    const auto doc = QJsonDocument::fromJson(proc.readAllStandardOutput());
    if (doc.isNull()) {
        return {};
    }
    m_videoInfoCache.insert(videoId, doc.object());
    return doc.object();
}

void YouTubeApi::splitFormats(const QJsonArray &ytdlpFormats, QJsonArray &adaptive, QJsonArray &combined)
{
    for (const auto &v : ytdlpFormats) {
        const auto f = v.toObject();
        const auto url = f.value(u"url"_s).toString();
        if (url.isEmpty()) {
            continue;
        }
        const auto vcodec = f.value(u"vcodec"_s).toString();
        const auto acodec = f.value(u"acodec"_s).toString();
        const bool hasVideo = !vcodec.isEmpty() && vcodec != u"none"_s;
        const bool hasAudio = !acodec.isEmpty() && acodec != u"none"_s;
        if (!hasVideo && !hasAudio) {
            continue;
        }

        QJsonObject entry{
            {u"url"_s, url},
            {u"itag"_s, f.value(u"format_id"_s).toString().toInt()},
            {u"container"_s, f.value(u"ext"_s).toString()},
            {u"clen"_s, static_cast<qint64>(f.value(u"filesize"_s).toDouble())},
        };
        if (hasVideo && hasAudio) {
            entry.insert(u"qualityLabel"_s, f.value(u"format_note"_s).toString());
            const int height = f.value(u"height"_s).toInt();
            if (height > 0) {
                entry.insert(u"resolution"_s, QStringLiteral("%1p").arg(height));
            }
            combined.append(entry);
        } else if (hasVideo) {
            entry.insert(u"encoding"_s, vcodec);
            entry.insert(u"qualityLabel"_s, f.value(u"format_note"_s).toString());
            const int height = f.value(u"height"_s).toInt();
            if (height > 0) {
                entry.insert(u"resolution"_s, QStringLiteral("%1p").arg(height));
            }
            adaptive.append(entry);
        } else {
            entry.insert(u"encoding"_s, acodec);
            adaptive.append(entry);
        }
    }
}

Error YouTubeApi::invalidJsonError()
{
    return {QNetworkReply::InternalServerError, i18n("Server returned no valid JSON.")};
}

Error YouTubeApi::notImplementedError()
{
    return {QNetworkReply::OperationNotImplementedError, i18n("This action is not supported for the YouTube source yet.")};
}

template<typename T>
QFuture<T> YouTubeApi::readyResult(T &&value)
{
    QFutureInterface<T> i;
    i.reportStarted();
    i.reportResult(std::forward<T>(value));
    i.reportFinished();
    return i.future();
}

QJsonObject YouTubeApi::baseContext() const
{
    QJsonObject client{
        {u"clientName"_s, QString::fromLatin1(CLIENT_NAME)},
        {u"clientVersion"_s, QString::fromLatin1(CLIENT_VERSION)},
        {u"hl"_s, m_language.isEmpty() ? u"en"_s : m_language},
        {u"gl"_s, m_region.isEmpty() ? u"US"_s : m_region},
    };
    return QJsonObject{{u"client"_s, client}};
}

QList<QJsonObject> YouTubeApi::walkVideoItems(const QJsonObject &root)
{
    QList<QJsonObject> items;
    QList<QJsonValue> stack{root};
    while (!stack.isEmpty()) {
        const auto v = stack.takeFirst();
        if (v.isObject()) {
            const auto obj = v.toObject();

            if (obj.contains(u"videoRenderer"_s)) {
                items << videoRendererToInvidious(obj.value(u"videoRenderer"_s).toObject());
                continue;
            }
            if (obj.contains(u"gridVideoRenderer"_s)) {
                items << videoRendererToInvidious(obj.value(u"gridVideoRenderer"_s).toObject());
                continue;
            }
            if (obj.contains(u"compactVideoRenderer"_s)) {
                items << videoRendererToInvidious(obj.value(u"compactVideoRenderer"_s).toObject());
                continue;
            }
            if (obj.contains(u"tileRenderer"_s)) {
                const auto tile = obj.value(u"tileRenderer"_s).toObject();
                if (tile.value(u"contentType"_s).toString() == u"TILE_CONTENT_TYPE_VIDEO"_s) {
                    items << tileRendererToInvidious(tile);
                }
                continue;
            }

            for (auto it = obj.begin(); it != obj.end(); ++it) {
                stack.append(it.value());
            }
        } else if (v.isArray()) {
            for (const auto &x : v.toArray()) {
                stack.append(x);
            }
        }
    }
    return items;
}

QList<VideoBasicInfo> YouTubeApi::parseVideoRenderers(const QJsonObject &root)
{
    QList<VideoBasicInfo> videos;
    for (const auto &item : walkVideoItems(root)) {
        videos << VideoBasicInfo::fromJson(item);
    }
    return videos;
}

QString YouTubeApi::flattenRuns(const QJsonValue &value)
{
    const auto obj = value.toObject();
    if (obj.contains(u"simpleText"_s)) {
        return obj.value(u"simpleText"_s).toString();
    }
    QString out;
    for (const auto &run : obj.value(u"runs"_s).toArray()) {
        out += run.toObject().value(u"text"_s).toString();
    }
    return out;
}

qint64 YouTubeApi::parseDigits(const QString &text)
{
    QString digits;
    for (QChar c : text) {
        if (c.isDigit()) {
            digits.append(c);
        }
    }
    return digits.toLongLong();
}

qint64 YouTubeApi::parseDuration(const QString &text)
{
    qint64 seconds = 0;
    for (const auto &part : text.split(u':')) {
        seconds = seconds * 60 + part.toInt();
    }
    return seconds;
}

QJsonObject YouTubeApi::videoRendererToInvidious(const QJsonObject &renderer)
{
    QJsonObject out;
    out.insert(u"type"_s, u"video"_s);
    out.insert(u"videoId"_s, renderer.value(u"videoId"_s).toString());
    out.insert(u"title"_s, flattenRuns(renderer.value(u"title"_s)));

    const auto owner = renderer.value(u"ownerText"_s).toObject();
    out.insert(u"author"_s, flattenRuns(owner));
    out.insert(u"authorId"_s,
               owner.value(u"runs"_s)
                   .toArray()
                   .first()
                   .toObject()
                   .value(u"navigationEndpoint"_s)
                   .toObject()
                   .value(u"browseEndpoint"_s)
                   .toObject()
                   .value(u"browseId"_s)
                   .toString());

    out.insert(u"viewCount"_s, parseDigits(flattenRuns(renderer.value(u"viewCountText"_s))));
    out.insert(u"lengthSeconds"_s, parseDuration(flattenRuns(renderer.value(u"lengthText"_s))));

    QJsonArray thumbnails;
    for (const auto &thumb : renderer.value(u"thumbnail"_s).toObject().value(u"thumbnails"_s).toArray()) {
        const auto t = thumb.toObject();
        QJsonObject mapped{
            {u"quality"_s, u"default"_s},
            {u"url"_s, t.value(u"url"_s).toString()},
            {u"width"_s, t.value(u"width"_s)},
            {u"height"_s, t.value(u"height"_s)},
        };
        thumbnails.append(mapped);
    }
    out.insert(u"videoThumbnails"_s, thumbnails);

    bool live = false;
    for (const auto &badge : renderer.value(u"badges"_s).toArray()) {
        const auto label = badge.toObject().value(u"metadataBadgeRenderer"_s).toObject().value(u"label"_s).toString();
        if (label.contains(u"LIVE"_s, Qt::CaseInsensitive)) {
            live = true;
            break;
        }
    }
    out.insert(u"liveNow"_s, live);
    return out;
}

QJsonObject YouTubeApi::tileRendererToInvidious(const QJsonObject &renderer)
{
    QJsonObject out;
    out.insert(u"type"_s, u"video"_s);

    const auto watchEndpoint = renderer.value(u"onSelectCommand"_s).toObject().value(u"watchEndpoint"_s).toObject();
    out.insert(u"videoId"_s, watchEndpoint.value(u"videoId"_s).toString());

    const auto metadata = renderer.value(u"metadata"_s).toObject().value(u"tileMetadataRenderer"_s).toObject();
    out.insert(u"title"_s, flattenRuns(metadata.value(u"title"_s)));

    const auto header = renderer.value(u"header"_s).toObject().value(u"tileHeaderRenderer"_s).toObject();

    QJsonArray thumbnails;
    for (const auto &thumb : header.value(u"thumbnail"_s).toObject().value(u"thumbnails"_s).toArray()) {
        const auto t = thumb.toObject();
        thumbnails.append(QJsonObject{
            {u"quality"_s, u"default"_s},
            {u"url"_s, t.value(u"url"_s).toString()},
            {u"width"_s, t.value(u"width"_s)},
            {u"height"_s, t.value(u"height"_s)},
        });
    }
    out.insert(u"videoThumbnails"_s, thumbnails);

    qint64 lengthSeconds = 0;
    bool live = false;
    for (const auto &overlay : header.value(u"thumbnailOverlays"_s).toArray()) {
        const auto timeStatus = overlay.toObject().value(u"thumbnailOverlayTimeStatusRenderer"_s).toObject();
        if (timeStatus.isEmpty()) {
            continue;
        }
        if (timeStatus.value(u"style"_s).toString() == u"LIVE"_s) {
            live = true;
        }
        lengthSeconds = parseDuration(flattenRuns(timeStatus.value(u"text"_s)));
    }
    out.insert(u"lengthSeconds"_s, lengthSeconds);
    out.insert(u"liveNow"_s, live);

    QString author;
    qint64 viewCount = 0;
    const auto lines = metadata.value(u"lines"_s).toArray();
    if (!lines.isEmpty()) {
        const auto firstLine = lines.first().toObject().value(u"lineRenderer"_s).toObject().value(u"items"_s).toArray();
        if (!firstLine.isEmpty()) {
            author = flattenRuns(firstLine.first().toObject().value(u"lineItemRenderer"_s).toObject().value(u"text"_s));
        }
    }
    if (lines.size() > 1) {
        for (const auto &it : lines.at(1).toObject().value(u"lineRenderer"_s).toObject().value(u"items"_s).toArray()) {
            const auto text = flattenRuns(it.toObject().value(u"lineItemRenderer"_s).toObject().value(u"text"_s));
            if (text.contains(u"view"_s, Qt::CaseInsensitive)) {
                viewCount = parseDigits(text);
            }
        }
    }
    out.insert(u"author"_s, author);
    out.insert(u"viewCount"_s, viewCount);

    return out;
}

template<typename T>
QFuture<T> YouTubeApi::innertubePost(const QString &endpoint, const QJsonObject &payload, std::function<T(QNetworkReply *)> process)
{
    QJsonObject body = payload;
    body.insert(u"context"_s, baseContext());

    QUrl url(QString::fromLatin1(INNERTUBE_HOST) % u"/youtubei/v1/" % endpoint);
    QUrlQuery query;
    query.addQueryItem(u"key"_s, QString::fromLatin1(INNERTUBE_API_KEY));
    query.addQueryItem(u"prettyPrint"_s, u"false"_s);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/json"));
    request.setHeader(QNetworkRequest::UserAgentHeader, QString::fromLatin1(USER_AGENT));
    request.setRawHeader("Origin", "https://www.youtube.com");
    request.setRawHeader("X-YouTube-Client-Name", QByteArray(CLIENT_NAME));
    request.setRawHeader("X-YouTube-Client-Version", QByteArray(CLIENT_VERSION));

    return post<T>(std::move(request), QJsonDocument(body).toJson(QJsonDocument::Compact), std::move(process));
}
