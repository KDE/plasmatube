// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

// While their API is (still) undocumented, the router can be found at
// https://github.com/TeamPiped/Piped-Backend/blob/6d0ad0698947ad3b4da72ec89d277c2f9a8ec187/src/main/java/me/kavin/piped/server/ServerLauncher.java

#include "pipedapi.h"

#include <KLocalizedString>

#include <QJsonDocument>
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
const QString API_SUBSCRIPTIONS = QStringLiteral("/subscriptions");

const QString TOKEN_KEY = QStringLiteral("token");

using namespace QInvidious;
using namespace Qt::StringLiterals;

PipedApi::PipedApi(QNetworkAccessManager *netManager, QObject *parent)
    : AbstractApi(netManager, parent)
{
}

bool PipedApi::isLoggedIn() const
{
    return m_token.has_value();
}

bool PipedApi::canLogIn() const
{
    return true;
}

void PipedApi::loadCredentials(const QString &prefix)
{
    if (const auto token = getKeychainValue(prefix, TOKEN_KEY)) {
        m_token = token;
    }
    Q_EMIT credentialsChanged();
}

void PipedApi::saveCredentials(const QString &prefix)
{
    if (const auto token = m_token) {
        setKeychainValue(prefix, TOKEN_KEY, *token);
    }
}

void PipedApi::wipeCredentials(const QString &prefix)
{
    wipeKeychainValue(prefix, TOKEN_KEY);
    m_token = std::nullopt;
    Q_EMIT credentialsChanged();
}

bool PipedApi::supportsFeature(AbstractApi::SupportedFeature feature)
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

QFuture<LogInResult> PipedApi::logIn(const QString &username, const QString &password)
{
    const QJsonObject object{
        {"username"_L1, username},
        {"password"_L1, password},
    };

    QUrl url = apiUrl(API_LOGIN);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/x-www-form-urlencoded"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::RedirectPolicy::ManualRedirectPolicy);

    return post<LogInResult>(std::move(request), QJsonDocument(object).toJson(), [this](QNetworkReply *reply) -> LogInResult {
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        if (document.object().contains("token"_L1)) {
            m_token = document.object()["token"_L1].toString();
            Q_EMIT credentialsChanged();
            return std::nullopt;
        }

        return std::pair(QNetworkReply::ContentAccessDenied, i18n("Username or password is wrong."));
    });
}

QFuture<VideoResult> PipedApi::requestVideo(const QString &videoId)
{
    return get<VideoResult>(QNetworkRequest(videoUrl(videoId)), [=](QNetworkReply *reply) -> VideoResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            auto video = Video::fromJson(doc);
            video.setVideoId(videoId); // Piped doesn't give us the video id in the response...
            return video;
        }
        return invalidJsonError();
    });
}

QFuture<SearchListResult> PipedApi::requestSearchResults(const SearchParameters &parameters, Paginator *paginator)
{
    Q_UNUSED(paginator)

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
    Q_UNUSED(paginator);
    return requestVideoList(Feed, QStringLiteral(""), {});
}

QFuture<VideoListResult> PipedApi::requestTrending(TrendingTopic topic, Paginator *paginator)
{
    Q_UNUSED(paginator)

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

QFuture<VideoListResult> PipedApi::requestChannel(const QString &query, Paginator *paginator)
{
    Q_UNUSED(paginator)
    return requestVideoList(Channel, query, {});
}

QFuture<SubscriptionsResult> PipedApi::requestSubscriptions()
{
    return get<SubscriptionsResult>(authenticatedNetworkRequest(apiUrl(API_SUBSCRIPTIONS)), [=](QNetworkReply *reply) -> SubscriptionsResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            auto array = doc.array();

            qWarning() << doc;

            QList<QString> subscriptions;
            std::transform(array.cbegin(), array.cend(), std::back_inserter(subscriptions), [](const QJsonValue &val) {
                return val.toObject().value(QStringLiteral("url")).toString().remove(QStringLiteral("/channel/"));
            });
            return subscriptions;
        }
        return invalidJsonError();
    });
}

QFuture<Result> PipedApi::subscribeToChannel(const QString &channel)
{
    // TODO: piped stub
    Q_UNUSED(channel)
    return {};
}

QFuture<Result> PipedApi::unsubscribeFromChannel(const QString &channel)
{
    // TODO: piped stub
    Q_UNUSED(channel)
    return {};
}

QFuture<HistoryResult> PipedApi::requestHistory(Paginator *paginator)
{
    // TODO: piped stub
    Q_UNUSED(paginator)
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

QFuture<CommentsResult> PipedApi::requestComments(const QString &videoId, Paginator *paginator)
{
    Q_UNUSED(paginator)

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
            return comments;
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

QFuture<ChannelResult> PipedApi::requestChannelInfo(const QString &queryd)
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
    Q_UNUSED(channelId)
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

QString PipedApi::getVideoUrl(const QString &videoId)
{
    return QStringLiteral("https://youtube.com/watch?v=%1").arg(videoId);
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
    if (isLoggedIn()) {
        request.setRawHeader(QByteArrayLiteral("Authorization"), m_token->toUtf8());
    }
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

QUrl PipedApi::videoUrl(const QString &videoId) const
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
    case Feed:
        urlString.append(API_FEED);
        break;
    }

    if (!urlExtension.isEmpty()) {
        urlString.append(QStringLiteral("/"));
        urlString.append(urlExtension);
    }

    for (QHash<QString, QString>::const_iterator parameter = parameters.begin(); parameter != parameters.end(); ++parameter) {
        query.addQueryItem(parameter.key(), parameter.value());
    }

    if (const auto token = m_token) {
        query.addQueryItem(QStringLiteral("authToken"), *token);
    }

    QUrl url = apiUrl(urlString);
    url.setQuery(query);
    return url;
}
