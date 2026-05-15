// SPDX-FileCopyrightText: 2026 Plasmatube Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "youtubeapi.h"

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
    return {};
}

QFuture<VideoResult> YouTubeApi::requestVideo(const QString &videoId)
{
    QJsonObject payload{{u"videoId"_s, videoId}};
    return innertubePost<VideoResult>(u"next"_s, payload, [videoId](QNetworkReply *reply) -> VideoResult {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            return invalidJsonError();
        }

        const auto contents = doc.object()
                                  .value(u"contents"_s)
                                  .toObject()
                                  .value(u"singleColumnWatchNextResults"_s)
                                  .toObject()
                                  .value(u"results"_s)
                                  .toObject()
                                  .value(u"results"_s)
                                  .toObject()
                                  .value(u"contents"_s)
                                  .toArray();

        QJsonObject synthesized;
        synthesized.insert(u"videoId"_s, videoId);
        for (const auto &c : contents) {
            const auto obj = c.toObject();
            if (obj.contains(u"itemSectionRenderer"_s)) {
                for (const auto &it : obj.value(u"itemSectionRenderer"_s).toObject().value(u"contents"_s).toArray()) {
                    const auto info = it.toObject().value(u"videoMetadataRenderer"_s).toObject();
                    if (!info.isEmpty()) {
                        synthesized.insert(u"title"_s, flattenRuns(info.value(u"title"_s)));
                        synthesized.insert(u"description"_s, flattenRuns(info.value(u"description"_s)));
                        synthesized.insert(u"viewCount"_s, flattenRuns(info.value(u"viewCount"_s)).remove(QRegularExpression(u"\\D"_s)).toLongLong());
                        const auto owner = info.value(u"owner"_s).toObject().value(u"videoOwnerRenderer"_s).toObject();
                        synthesized.insert(u"author"_s, flattenRuns(owner.value(u"title"_s)));
                        synthesized.insert(
                            u"authorId"_s,
                            owner.value(u"navigationEndpoint"_s).toObject().value(u"browseEndpoint"_s).toObject().value(u"browseId"_s).toString());
                    }
                }
            }
        }
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
        const auto sections = doc.object().value(u"contents"_s).toObject().value(u"sectionListRenderer"_s).toObject().value(u"contents"_s).toArray();
        for (const auto &section : sections) {
            const auto items = section.toObject().value(u"itemSectionRenderer"_s).toObject().value(u"contents"_s).toArray();
            for (const auto &item : items) {
                const auto renderer = item.toObject().value(u"videoRenderer"_s).toObject();
                if (renderer.isEmpty()) {
                    continue;
                }
                results << SearchResult::fromJson(videoRendererToInvidious(renderer));
            }
        }
        return results;
    });
}

QFuture<VideoListResult> YouTubeApi::requestFeed(Paginator *paginator)
{
    Q_UNUSED(paginator)
    return {};
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
    return {};
}

QFuture<Result> YouTubeApi::subscribeToChannel(const QString &channel)
{
    Q_UNUSED(channel)
    return {};
}

QFuture<Result> YouTubeApi::unsubscribeFromChannel(const QString &channel)
{
    Q_UNUSED(channel)
    return {};
}

QFuture<HistoryResult> YouTubeApi::requestHistory(Paginator *paginator)
{
    Q_UNUSED(paginator)
    return {};
}

QFuture<Result> YouTubeApi::markWatched(const QString &videoId)
{
    Q_UNUSED(videoId)
    return {};
}

QFuture<Result> YouTubeApi::markUnwatched(const QString &videoId)
{
    Q_UNUSED(videoId)
    return {};
}

QFuture<CommentsResult> YouTubeApi::requestComments(const QString &videoId, Paginator *paginator)
{
    Q_UNUSED(videoId)
    Q_UNUSED(paginator)
    return {};
}

QFuture<PlaylistsResult> YouTubeApi::requestPlaylists()
{
    return {};
}

QFuture<PreferencesResult> YouTubeApi::requestPreferences()
{
    return {};
}

QFuture<Result> YouTubeApi::setPreferences(const QInvidious::Preferences &preferences)
{
    Q_UNUSED(preferences)
    return {};
}

QFuture<VideoListResult> YouTubeApi::requestPlaylist(const QString &plid)
{
    Q_UNUSED(plid)
    return {};
}

QFuture<ChannelResult> YouTubeApi::requestChannelInfo(const QString &queryd)
{
    Q_UNUSED(queryd)
    return {};
}

QFuture<PlaylistsResult> YouTubeApi::requestChannelPlaylists(const QString &channelId)
{
    Q_UNUSED(channelId)
    return {};
}

QFuture<Result> YouTubeApi::addVideoToPlaylist(const QString &plid, const QString &videoId)
{
    Q_UNUSED(plid)
    Q_UNUSED(videoId)
    return {};
}

QFuture<Result> YouTubeApi::removeVideoFromPlaylist(const QString &plid, const QString &indexId)
{
    Q_UNUSED(plid)
    Q_UNUSED(indexId)
    return {};
}

QString YouTubeApi::getVideoUrl(const QString &videoId)
{
    return QStringLiteral("https://youtube.com/watch?v=%1").arg(videoId);
}

Error YouTubeApi::invalidJsonError()
{
    return {QNetworkReply::InternalServerError, i18n("Server returned no valid JSON.")};
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

QList<VideoBasicInfo> YouTubeApi::parseVideoRenderers(const QJsonObject &root)
{
    QList<VideoBasicInfo> videos;
    QList<QJsonValue> stack{root};
    while (!stack.isEmpty()) {
        const auto v = stack.takeFirst();
        if (v.isObject()) {
            const auto obj = v.toObject();
            const auto rendererName = obj.contains(u"videoRenderer"_s) ? u"videoRenderer"_s
                : obj.contains(u"gridVideoRenderer"_s)                 ? u"gridVideoRenderer"_s
                : obj.contains(u"compactVideoRenderer"_s)              ? u"compactVideoRenderer"_s
                                                                       : QString();
            if (!rendererName.isEmpty()) {
                videos << VideoBasicInfo::fromJson(videoRendererToInvidious(obj.value(rendererName).toObject()));
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

    const auto viewText = flattenRuns(renderer.value(u"viewCountText"_s));
    QString digits;
    for (QChar c : viewText) {
        if (c.isDigit()) {
            digits.append(c);
        }
    }
    out.insert(u"viewCount"_s, digits.toLongLong());

    const auto lengthText = flattenRuns(renderer.value(u"lengthText"_s));
    qint64 seconds = 0;
    for (const auto &part : lengthText.split(u':')) {
        seconds = seconds * 60 + part.toInt();
    }
    out.insert(u"lengthSeconds"_s, seconds);

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
