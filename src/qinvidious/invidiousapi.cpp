// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "invidiousapi.h"
// Qt
#include <QFutureInterface>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringBuilder>
#include <QUrl>
#include <QUrlQuery>

#include <KLocalizedString>

constexpr QStringView API_FEED = u"/api/v1/auth/feed";
constexpr QStringView API_LOGIN = u"/login";
constexpr QStringView API_SEARCH = u"/api/v1/search";
constexpr QStringView API_SUBSCRIPTIONS = u"/api/v1/auth/subscriptions";
constexpr QStringView API_TOP = u"/api/v1/top";
constexpr QStringView API_TRENDING = u"/api/v1/trending";
constexpr QStringView API_VIDEOS = u"/api/v1/videos";
constexpr QStringView API_CHANNELS = u"/api/v1/channels/videos";
constexpr QStringView API_HISTORY = u"/api/v1/auth/history";
constexpr QStringView API_COMMENTS = u"/api/v1/comments";
constexpr QStringView API_LIST_PLAYLISTS = u"/api/v1/auth/playlists";
constexpr QStringView API_PREFERENCES = u"/api/v1/auth/preferences";
constexpr QStringView API_PLAYLISTS = u"/api/v1/playlists";
constexpr QStringView API_CHANNEL_INFO = u"/api/v1/channels";

using namespace QInvidious;
using namespace Qt::StringLiterals;

InvidiousApi::InvidiousApi(QNetworkAccessManager *netManager, QObject *parent)
    : QObject(parent),
      m_netManager(netManager)
{
}

QString InvidiousApi::region() const
{
    return m_region;
}

void InvidiousApi::setRegion(const QString &region)
{
    m_region = region;
}

QString InvidiousApi::language() const
{
    return m_language;
}

void InvidiousApi::setLanguage(const QString &language)
{
    m_language = language;
}

Credentials InvidiousApi::credentials() const
{
    return m_credentials;
}

void InvidiousApi::setCredentials(const Credentials &credentials)
{
    m_credentials = credentials;
    Q_EMIT credentialsChanged();
}

void InvidiousApi::setCredentials(const QString &apiInstance)
{
    m_credentials = Credentials();
    m_credentials.setApiInstance(apiInstance);
    Q_EMIT credentialsChanged();
}

QString InvidiousApi::invidiousInstance() const
{
    return m_credentials.apiInstance();
}

QFuture<LogInResult> InvidiousApi::logIn(QStringView username, QStringView password)
{
    QUrlQuery params;
    params.addQueryItem(QStringLiteral("email"), QString::fromUtf8(QUrl::toPercentEncoding(username.toString())));
    params.addQueryItem(QStringLiteral("password"), QString::fromUtf8(QUrl::toPercentEncoding(password.toString())));
    params.addQueryItem(QStringLiteral("action"), QStringLiteral("signin"));

    QNetworkRequest request(logInUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/x-www-form-urlencoded"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::RedirectPolicy::ManualRedirectPolicy);

    return post<LogInResult>(std::move(request), params.toString().toUtf8(),
                             [=](QNetworkReply *reply) -> LogInResult {
        const auto cookies = reply->header(QNetworkRequest::SetCookieHeader).value<QList<QNetworkCookie>>();

        if (!cookies.isEmpty()) {
            m_credentials.setUsername(username);
            m_credentials.setCookie(cookies.first());
            Q_EMIT credentialsChanged();
            return m_credentials;
        }
        return std::pair(QNetworkReply::ContentAccessDenied, i18n("Username or password is wrong."));
    });
}

QFuture<VideoResult> InvidiousApi::requestVideo(QStringView videoId)
{
    return get<VideoResult>(QNetworkRequest(videoUrl(videoId)),
                            [=](QNetworkReply *reply) -> VideoResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            return Video::fromJson(doc);
        }
        return invalidJsonError();
    });
}

QFuture<VideoListResult> InvidiousApi::requestSearchResults(const SearchParameters &parameters)
{
    return requestVideoList(Search, QStringLiteral(""), parameters.toQueryParameters());
}

QFuture<VideoListResult> InvidiousApi::requestFeed(qint32 page) {
    QHash<QString, QString> parameters;
    parameters.insert(QStringLiteral("page"), QString::number(page));

    return requestVideoList(Feed, QStringLiteral(""), parameters);
}

QFuture<VideoListResult> InvidiousApi::requestTop()
{
    return requestVideoList(Top);
}

QFuture<VideoListResult> InvidiousApi::requestTrending(TrendingTopic topic)
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

QFuture<VideoListResult> InvidiousApi::requestChannel(QStringView query, qint32 page)
{

    QHash<QString, QString> parameters;
    parameters.insert(QStringLiteral("page"), QString::number(page));
    return requestVideoList(Channel, query.toString(), parameters);
}

QFuture<SubscriptionsResult> InvidiousApi::requestSubscriptions()
{
    return get<SubscriptionsResult>(authenticatedNetworkRequest(subscriptionsUrl()),
                                    [=](QNetworkReply *reply) -> SubscriptionsResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            auto array = doc.array();

            QList<QString> subscriptions;
            std::transform(array.cbegin(), array.cend(), std::back_inserter(subscriptions),
                           [](const QJsonValue &val) {
                return val.toObject().value(QStringLiteral("authorId")).toString();
            });
            return subscriptions;
        }
        return invalidJsonError();
    });
}

QFuture<Result> InvidiousApi::subscribeToChannel(QStringView channel)
{
    return post<Result>(authenticatedNetworkRequest(subscribeUrl(channel)), {}, checkIsReplyOk);
}

QFuture<Result> InvidiousApi::unsubscribeFromChannel(QStringView channel)
{
    return deleteResource<Result>(authenticatedNetworkRequest(subscribeUrl(channel)), checkIsReplyOk);
}

QFuture<HistoryResult> InvidiousApi::requestHistory(qint32 page)
{
    QUrlQuery parameters;
    parameters.addQueryItem(QStringLiteral("page"), QString::number(page));

    QUrl url{invidiousInstance() % API_HISTORY};
    url.setQuery(parameters);

    return get<HistoryResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> HistoryResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto array = doc.array();

            QList<QString> history;
            std::transform(array.cbegin(), array.cend(), std::back_inserter(history), [](const QJsonValue &val) {
                return val.toString();
            });
            return history;
        }
        return invalidJsonError();
    });
}

QFuture<Result> InvidiousApi::markWatched(const QString &videoId)
{
    return post<Result>(authenticatedNetworkRequest(QUrl(invidiousInstance() % API_HISTORY % u'/' % videoId)), {}, checkIsReplyOk);
}

QFuture<Result> InvidiousApi::markUnwatched(const QString &videoId)
{
    return deleteResource<Result>(authenticatedNetworkRequest(QUrl(invidiousInstance() % API_HISTORY % u'/' % videoId)), checkIsReplyOk);
}

QFuture<CommentsResult> InvidiousApi::requestComments(const QString &videoId, const QString &continuation)
{
    QUrlQuery parameters;
    if (!continuation.isEmpty()) {
        parameters.addQueryItem(QStringLiteral("continuation"), continuation);
    }

    QUrl url{invidiousInstance() % API_COMMENTS % u'/' % videoId};
    url.setQuery(parameters);

    return get<CommentsResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> CommentsResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto array = doc[u"comments"].toArray();

            QList<Comment> comments;
            std::transform(array.cbegin(), array.cend(), std::back_inserter(comments), [](const QJsonValue &val) {
                Comment comment;
                Comment::fromJson(val.toObject(), comment);
                return comment;
            });
            return Comments{comments, doc[u"continuation"].toString()};
        }
        return invalidJsonError();
    });
}

QFuture<PlaylistsResult> InvidiousApi::requestPlaylists()
{
    QUrl url{invidiousInstance() % API_LIST_PLAYLISTS};

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

QFuture<PreferencesResult> InvidiousApi::requestPreferences()
{
    QUrl url{invidiousInstance() % API_PREFERENCES};

    return get<PreferencesResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> PreferencesResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            Preferences preferences;
            Preferences::fromJson(doc.object(), preferences);
            return preferences;
        }
        return invalidJsonError();
    });
}

QFuture<Result> InvidiousApi::setPreferences(const QInvidious::Preferences &preferences)
{
    QUrl url{invidiousInstance() % API_PREFERENCES};

    return post<Result>(authenticatedNetworkRequest(std::move(url)), QJsonDocument(preferences.toJson()).toJson(), checkIsReplyOk);
}

QFuture<VideoListResult> InvidiousApi::requestPlaylist(const QString &plid)
{
    QUrl url{invidiousInstance() % API_PLAYLISTS % u'/' % plid};

    return get<VideoListResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> VideoListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto obj = doc.object();

            return VideoBasicInfo::fromJson(obj.value("videos"_L1).toArray());
        }
        return invalidJsonError();
    });
}

QFuture<ChannelResult> InvidiousApi::requestChannelInfo(QStringView queryd)
{
    QUrl url{invidiousInstance() % API_CHANNEL_INFO % u'/' % queryd};

    return get<ChannelResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> ChannelResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            QInvidious::Channel channel;
            Channel::fromJson(doc.object(), channel);
            return channel;
        }
        return invalidJsonError();
    });
}

QFuture<Result> InvidiousApi::addVideoToPlaylist(const QString &plid, const QString &videoId)
{
    QUrl url{invidiousInstance() % API_LIST_PLAYLISTS % u'/' % plid % u"/videos"};

    QJsonObject requestObj;
    requestObj["videoId"_L1] = videoId;

    QNetworkRequest request = authenticatedNetworkRequest(std::move(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/json"));

    return post<Result>(std::move(request), QJsonDocument(requestObj).toJson(QJsonDocument::Compact), checkIsReplyOk);
}

QFuture<Result> InvidiousApi::removeVideoFromPlaylist(const QString &plid, const QString &indexId)
{
    QUrl url{invidiousInstance() % API_LIST_PLAYLISTS % u'/' % plid % u"/videos/" % indexId};

    return deleteResource<Result>(authenticatedNetworkRequest(std::move(url)), checkIsReplyOk);
}

Error InvidiousApi::invalidJsonError()
{
    return std::pair(QNetworkReply::InternalServerError, i18n("Server returned no valid JSON."));
}

Result InvidiousApi::checkIsReplyOk(QNetworkReply *reply)
{
    auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status >= 200 && status < 300) {
        return Success();
    }
    return std::pair(QNetworkReply::InternalServerError, i18n("Server returned the status code %1", QString::number(status)));
}

QFuture<VideoListResult> InvidiousApi::requestVideoList(VideoListType queryType, const QString &urlExtension, const QHash<QString, QString> &parameters)
{
    auto url = videoListUrl(queryType, urlExtension, parameters);
    // Feed requests require to be authenticated
    auto request = queryType == Feed ? authenticatedNetworkRequest(std::move(url))
                                     : QNetworkRequest(url);

    return get<VideoListResult>(std::move(request), [=](QNetworkReply *reply) -> VideoListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            if (queryType == Feed) {
                const auto obj = doc.object();

              // add videos marked as notification
                auto results = VideoBasicInfo::fromJson(obj.value("notifications"_L1).toArray());
                for (auto &video : results) {
                    video.setIsNotification(true);
                }

                // add the rest
                results << VideoBasicInfo::fromJson(obj.value("videos"_L1).toArray());
                return results;
            } else if (queryType == Channel) {
              const auto obj = doc.object();

              auto results = VideoBasicInfo::fromJson(obj.value("videos"_L1).toArray());
              return results;
            } else {
              QList<VideoBasicInfo> results;
              for (auto value : doc.array()) {
                  if (value.isObject() && value.toObject()["type"_L1] == "video"_L1) {
                      results << VideoBasicInfo::fromJson(value.toObject());
                  }
              }

              return results;
            }
        }
        return invalidJsonError();
    });
}

QNetworkRequest InvidiousApi::authenticatedNetworkRequest(QUrl &&url)
{
    QNetworkRequest request(url);
    if (!m_credentials.isAnonymous()) {
        const QList<QNetworkCookie> cookies {
            m_credentials.cookie().value()
        };
        request.setHeader(QNetworkRequest::CookieHeader, QVariant::fromValue(cookies));
    }
    // some invidious instances redirect some calls using reverse proxies
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    return request;
}

QUrlQuery InvidiousApi::genericUrlQuery() const
{
    QUrlQuery query;
    if (!m_language.isEmpty()) {
        query.addQueryItem(QStringLiteral("hl"), m_language);
    }
    if (!m_region.isEmpty()) {
        query.addQueryItem(QStringLiteral("region"), m_region);
    }
    return query;
}

QUrl InvidiousApi::logInUrl() const
{
    QUrl url(invidiousInstance() % API_LOGIN);
    auto urlQuery = genericUrlQuery();
    urlQuery.addQueryItem(QStringLiteral("referer"), QString::fromUtf8(QUrl::toPercentEncoding(QStringLiteral("/"))));
    urlQuery.addQueryItem(QStringLiteral("type"), QStringLiteral("invidious"));
    url.setQuery(urlQuery);
    return url;
}

QUrl InvidiousApi::videoUrl(QStringView videoId) const
{
    return QUrl(invidiousInstance() % API_VIDEOS % u'/' % videoId);
}

QUrl InvidiousApi::videoListUrl(VideoListType queryType, const QString &urlExtension, const QHash<QString, QString>& parameters) const
{
    auto urlString = invidiousInstance();
    auto query = genericUrlQuery();

    switch (queryType) {
    case Search:
        urlString.append(API_SEARCH);
        break;
    case Trending:
        urlString.append(API_TRENDING);
        break;
    case Top:
        urlString.append(API_TOP);
        break;
    case Feed:
        urlString.append(API_FEED);
        break;
    case Channel:
        urlString.append(API_CHANNELS);
        break;
    }

    if (!urlExtension.isEmpty())
    {
        urlString.append(QStringLiteral("/"));
        urlString.append(urlExtension);
    }

    for (QHash<QString, QString>::const_iterator parameter = parameters.begin(); parameter != parameters.end(); ++parameter) {
        query.addQueryItem(parameter.key(), parameter.value());
    }

    QUrl url(urlString);
    url.setQuery(query);
    return url;
}

QUrl InvidiousApi::subscriptionsUrl() const
{
    auto url = QUrl(invidiousInstance() % API_SUBSCRIPTIONS);
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("fields"), QStringLiteral("authorId"));
    url.setQuery(query);
    return url;

}

QUrl InvidiousApi::subscribeUrl(QStringView channelId) const
{
    return QUrl(invidiousInstance() % API_SUBSCRIPTIONS % u'/' % channelId);
}
