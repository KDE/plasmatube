// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pipedapi.h"

#include <KLocalizedString>

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringBuilder>
#include <QUrl>
#include <QUrlQuery>

const QString API_FEED = QStringLiteral("/feed");
const QString API_LOGIN = QStringLiteral("/login");
const QString API_SEARCH = QStringLiteral("/search");
const QString API_TRENDING = QStringLiteral("/trending");
const QString API_VIDEOS = QStringLiteral("/streams");
const QString API_CHANNELS = QStringLiteral("/channel");
const QString API_COMMENTS = QStringLiteral("/comments");
const QString API_PLAYLISTS = QStringLiteral("/playlists");
const QString API_CHANNEL_INFO = QStringLiteral("/channel");

using namespace QInvidious;
using namespace Qt::StringLiterals;

PipedApi::PipedApi(QNetworkAccessManager *netManager, QObject *parent)
    : AbstractApi(netManager, parent)
{
}

bool PipedApi::isLoggedIn() const
{
    return false;
}

bool PipedApi::canLogIn() const
{
    return false;
}

void PipedApi::loadCredentials(const QString &prefix)
{
}

void PipedApi::saveCredentials(const QString &prefix)
{
}

void PipedApi::wipeCredentials(const QString &prefix)
{
}

bool PipedApi::supportsFeature(AbstractApi::SupportedFeature feature)
{
    switch (feature) {
    case PopularPage:
    case TrendingCategories:
        return false;
    }

    return false;
}

QFuture<LogInResult> PipedApi::logIn(QStringView username, QStringView password)
{
    QUrlQuery params;
    params.addQueryItem(QStringLiteral("email"), QString::fromUtf8(QUrl::toPercentEncoding(username.toString())));
    params.addQueryItem(QStringLiteral("password"), QString::fromUtf8(QUrl::toPercentEncoding(password.toString())));
    params.addQueryItem(QStringLiteral("action"), QStringLiteral("signin"));

    QUrl url = apiUrl(API_LOGIN);
    auto urlQuery = genericUrlQuery();
    urlQuery.addQueryItem(QStringLiteral("referer"), QString::fromUtf8(QUrl::toPercentEncoding(QStringLiteral("/"))));
    urlQuery.addQueryItem(QStringLiteral("type"), QStringLiteral("invidious"));
    url.setQuery(urlQuery);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/x-www-form-urlencoded"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::RedirectPolicy::ManualRedirectPolicy);

    return post<LogInResult>(std::move(request), params.toString().toUtf8(), [=](QNetworkReply *reply) -> LogInResult {
        return std::pair(QNetworkReply::ContentAccessDenied, i18n("Username or password is wrong."));
    });
}

QFuture<VideoResult> PipedApi::requestVideo(QStringView videoId)
{
    return get<VideoResult>(QNetworkRequest(videoUrl(videoId)), [=](QNetworkReply *reply) -> VideoResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            auto video = Video::fromJson(doc);
            video.setVideoId(videoId.toString()); // Piped doesn't give us the video id in the respone...
            return video;
        }
        return invalidJsonError();
    });
}

QString PipedApi::resolveVideoUrl(QStringView videoId)
{
    return QStringLiteral("ytdl://%1").arg(videoId);
}

QFuture<SearchListResult> PipedApi::requestSearchResults(const SearchParameters &parameters)
{
    auto url = videoListUrl(Search, QStringLiteral(""), parameters.toPipedQueryParameters());
    auto request = QNetworkRequest(url);

    return get<SearchListResult>(std::move(request), [=](QNetworkReply *reply) -> SearchListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto obj = doc.object();

            QList<SearchResult> results;
            for (auto value : doc["items"_L1].toArray()) {
                if (value.isObject()) {
                    results << SearchResult::fromJson(value.toObject());
                }
            }

            return results;
        }
        return invalidJsonError();
    });
}

QFuture<VideoListResult> PipedApi::requestFeed(Paginator *paginator)
{
    // TODO: piped stub
    return {};
}

QFuture<VideoListResult> PipedApi::requestTop()
{
    return {};
}

QFuture<VideoListResult> PipedApi::requestTrending(TrendingTopic topic, Paginator *paginator)
{
    QHash<QString, QString> parameters;
    switch (topic) {
    case Music:
        parameters.insert(QStringLiteral("type"), QStringLiteral("music"));
        break;
    case Gaming:
        parameters.insert(QStringLiteral("type"), QStringLiteral("gaming"));
        break;
    case Movies:
        parameters.insert(QStringLiteral("type"), QStringLiteral("movies"));
        break;
    case News:
        parameters.insert(QStringLiteral("type"), QStringLiteral("news"));
        break;
    case Main:
        break;
    }
    return requestVideoList(Trending, QStringLiteral(""), parameters);
}

QFuture<VideoListResult> PipedApi::requestChannel(QStringView query, qint32 page)
{
    Q_UNUSED(page)
    return requestVideoList(Channel, query.toString(), {});
}

QFuture<SubscriptionsResult> PipedApi::requestSubscriptions()
{
    // TODO: piped stub
    return {};
}

QFuture<Result> PipedApi::subscribeToChannel(QStringView channel)
{
    // TODO: piped stub
    Q_UNUSED(channel)
    return {};
}

QFuture<Result> PipedApi::unsubscribeFromChannel(QStringView channel)
{
    // TODO: piped stub
    Q_UNUSED(channel)
    return {};
}

QFuture<HistoryResult> PipedApi::requestHistory(qint32 page)
{
    // TODO: piped stub
    Q_UNUSED(page)
    return {};
}

QFuture<Result> PipedApi::markWatched(const QString &videoId)
{
    // TODO: piped stub
    Q_UNUSED(videoId)
    return {};
}

QFuture<Result> PipedApi::markUnwatched(const QString &videoId)
{
    // TODO: piped stub
    Q_UNUSED(videoId)
    return {};
}

QFuture<CommentsResult> PipedApi::requestComments(const QString &videoId, const QString &continuation)
{
    Q_UNUSED(continuation)

    if (videoId.isEmpty()) {
        qWarning() << "Not trying to load comments for an empty video id.";
        return {};
    }

    QUrl url = apiUrl(API_COMMENTS % u'/' % videoId);

    return get<CommentsResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> CommentsResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto array = doc[u"comments"].toArray();

            QList<Comment> comments;
            std::transform(array.cbegin(), array.cend(), std::back_inserter(comments), [](const QJsonValue &val) {
                Comment comment;
                Comment::fromJson(val.toObject(), comment);
                return comment;
            });
            return Comments{comments, {}};
        }
        return invalidJsonError();
    });
}

QFuture<PlaylistsResult> PipedApi::requestPlaylists()
{
    QUrl url = apiUrl(API_PLAYLISTS);

    return get<PlaylistsResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> PlaylistsResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto array = doc.array();

            QList<Playlist> playlists;
            std::transform(array.cbegin(), array.cend(), std::back_inserter(playlists), [](const QJsonValue &val) {
                Playlist playlist;
                Playlist::fromJson(val.toObject(), playlist);
                return playlist;
            });
            return playlists;
        }
        return invalidJsonError();
    });
}

QFuture<PreferencesResult> PipedApi::requestPreferences()
{
    // TODO: piped stub
    return {};
}

QFuture<Result> PipedApi::setPreferences(const QInvidious::Preferences &preferences)
{
    // TODO: piped stub
    Q_UNUSED(preferences)
    return {};
}

QFuture<VideoListResult> PipedApi::requestPlaylist(const QString &plid)
{
    QUrl url = apiUrl(API_PLAYLISTS % u'/' % plid);

    return get<VideoListResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> VideoListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto obj = doc.object();

            // TODO: handle pagination
            return VideoBasicInfo::fromJson(obj.value("relatedStreams"_L1).toArray());
        }
        return invalidJsonError();
    });
}

QFuture<ChannelResult> PipedApi::requestChannelInfo(QStringView queryd)
{
    QUrl url = apiUrl(API_CHANNEL_INFO % u'/' % queryd);

    return get<ChannelResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> ChannelResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            QInvidious::Channel channel;
            Channel::fromJson(doc.object(), channel);
            return channel;
        }
        return invalidJsonError();
    });
}

QFuture<PlaylistsResult> PipedApi::requestChannelPlaylists(const QString &channelId)
{
    // TODO: piped stub
    return {};
}

QFuture<Result> PipedApi::addVideoToPlaylist(const QString &plid, const QString &videoId)
{
    // TODO: piped stub
    Q_UNUSED(plid)
    Q_UNUSED(videoId)
    return {};
}

QFuture<Result> PipedApi::removeVideoFromPlaylist(const QString &plid, const QString &indexId)
{
    // TODO: piped stub
    Q_UNUSED(plid)
    Q_UNUSED(indexId)
    return {};
}

Error PipedApi::invalidJsonError()
{
    return {QNetworkReply::InternalServerError, i18n("Server returned no valid JSON.")};
}

Result PipedApi::checkIsReplyOk(QNetworkReply *reply)
{
    auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status >= 200 && status < 300) {
        return Success();
    }
    return std::pair(QNetworkReply::InternalServerError, i18n("Server returned the status code %1", QString::number(status)));
}

QFuture<VideoListResult> PipedApi::requestVideoList(VideoListType queryType, const QString &urlExtension, const QHash<QString, QString> &parameters)
{
    auto url = videoListUrl(queryType, urlExtension, parameters);
    // Feed requests require to be authenticated
    auto request = QNetworkRequest(url);

    return get<VideoListResult>(std::move(request), [=](QNetworkReply *reply) -> VideoListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            if (queryType == Channel) {
                const auto obj = doc.object();

                auto results = VideoBasicInfo::fromJson(obj.value("relatedStreams"_L1).toArray());
                return results;
            } else if (queryType == Search) {
                const auto obj = doc.object();

                auto results = obj.value("items"_L1).toArray();
                QList<VideoBasicInfo> videoResults;
                for (auto value : results) {
                    if (value.isObject() && value.toObject()["type"_L1] == "stream"_L1) {
                        videoResults << VideoBasicInfo::fromJson(value.toObject());
                    }
                }

                return videoResults;
            } else {
                QList<VideoBasicInfo> results;
                for (auto value : doc.array()) {
                    if (value.isObject() && value.toObject()["type"_L1] == "stream"_L1) {
                        results << VideoBasicInfo::fromJson(value.toObject());
                    }
                }

                return results;
            }
        }
        return invalidJsonError();
    });
}

QNetworkRequest PipedApi::authenticatedNetworkRequest(QUrl &&url)
{
    QNetworkRequest request(url);
    return request;
}

QUrlQuery PipedApi::genericUrlQuery() const
{
    QUrlQuery query;
    if (!m_language.isEmpty()) {
        query.addQueryItem(QStringLiteral("hl"), m_language);
    }
    // TODO: make configurable
    query.addQueryItem(QStringLiteral("region"), QStringLiteral("US"));
    return query;
}

QUrl PipedApi::videoUrl(QStringView videoId) const
{
    return apiUrl(API_VIDEOS % u'/' % videoId);
}

QUrl PipedApi::videoListUrl(VideoListType queryType, const QString &urlExtension, const QHash<QString, QString> &parameters) const
{
    QString urlString;
    auto query = genericUrlQuery();

    switch (queryType) {
    case Search:
        urlString.append(API_SEARCH);
        break;
    case Trending:
        urlString.append(API_TRENDING);
        break;
    case Channel:
        urlString.append(API_CHANNELS);
        break;
    }

    if (!urlExtension.isEmpty()) {
        urlString.append(QStringLiteral("/"));
        urlString.append(urlExtension);
    }

    for (QHash<QString, QString>::const_iterator parameter = parameters.begin(); parameter != parameters.end(); ++parameter) {
        query.addQueryItem(parameter.key(), parameter.value());
    }

    QUrl url = apiUrl(urlString);
    url.setQuery(query);
    return url;
}
