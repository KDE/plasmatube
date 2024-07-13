// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "invidiousapi.h"

#include <KLocalizedString>

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringBuilder>
#include <QUrl>
#include <QUrlQuery>

const QString API_FEED = QStringLiteral("/api/v1/auth/feed");
const QString API_LOGIN = QStringLiteral("/login");
const QString API_SEARCH = QStringLiteral("/api/v1/search");
const QString API_SUBSCRIPTIONS = QStringLiteral("/api/v1/auth/subscriptions");
const QString API_TOP = QStringLiteral("/api/v1/top");
const QString API_TRENDING = QStringLiteral("/api/v1/trending");
const QString API_VIDEOS = QStringLiteral("/api/v1/videos");
const QString API_CHANNEL_VIDEOS = QStringLiteral("/api/v1/channels/videos");
const QString API_CHANNEL_PLAYLISTS = QStringLiteral("/api/v1/channels/playlists");
const QString API_HISTORY = QStringLiteral("/api/v1/auth/history");
const QString API_COMMENTS = QStringLiteral("/api/v1/comments");
const QString API_LIST_PLAYLISTS = QStringLiteral("/api/v1/auth/playlists");
const QString API_PREFERENCES = QStringLiteral("/api/v1/auth/preferences");
const QString API_PLAYLISTS = QStringLiteral("/api/v1/playlists");
const QString API_CHANNEL_INFO = QStringLiteral("/api/v1/channels");

const QString COOKIE_KEY = QStringLiteral("cookie");

using namespace QInvidious;
using namespace Qt::StringLiterals;

InvidiousApi::InvidiousApi(QNetworkAccessManager *netManager, QObject *parent)
    : AbstractApi(netManager, parent)
{
}

bool InvidiousApi::isLoggedIn() const
{
    return m_cookie.has_value();
}

bool InvidiousApi::canLogIn() const
{
    // TODO: can we check if login is disabled, is that even an option?
    return true;
}

void InvidiousApi::loadCredentials(const QString &prefix)
{
    if (auto cookie = getKeychainValue(prefix, COOKIE_KEY)) {
        m_cookie = QNetworkCookie(cookie->toUtf8());
    }
    Q_EMIT credentialsChanged();
}

void InvidiousApi::saveCredentials(const QString &prefix)
{
    if (auto cookie = m_cookie) {
        setKeychainValue(prefix, COOKIE_KEY, QString::fromUtf8(cookie->toRawForm()));
    }
}

void InvidiousApi::wipeCredentials(const QString &prefix)
{
    wipeKeychainValue(prefix, COOKIE_KEY);
    m_cookie = std::nullopt;
    Q_EMIT credentialsChanged();
}

bool InvidiousApi::supportsFeature(AbstractApi::SupportedFeature feature)
{
    switch (feature) {
    case PopularPage:
    case TrendingCategories:
        return true;
    }

    return false;
}

QFuture<LogInResult> InvidiousApi::logIn(const QString &username, const QString &password)
{
    QUrlQuery params;
    params.addQueryItem(QStringLiteral("email"), QString::fromUtf8(QUrl::toPercentEncoding(username)));
    params.addQueryItem(QStringLiteral("password"), QString::fromUtf8(QUrl::toPercentEncoding(password)));
    params.addQueryItem(QStringLiteral("action"), QStringLiteral("signin"));

    QUrl url = apiUrl(API_LOGIN);
    auto urlQuery = genericUrlQuery();
    urlQuery.addQueryItem(QStringLiteral("referer"), QString::fromUtf8(QUrl::toPercentEncoding(QStringLiteral("/"))));
    urlQuery.addQueryItem(QStringLiteral("type"), QStringLiteral("invidious"));
    url.setQuery(urlQuery);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/x-www-form-urlencoded"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::RedirectPolicy::ManualRedirectPolicy);

    return post<LogInResult>(std::move(request), params.toString().toUtf8(), [this](QNetworkReply *reply) -> LogInResult {
        const auto cookies = reply->header(QNetworkRequest::SetCookieHeader).value<QList<QNetworkCookie>>();

        if (!cookies.isEmpty()) {
            m_cookie = cookies.first();
            Q_EMIT credentialsChanged();
            return std::nullopt;
        }
        return std::pair(QNetworkReply::ContentAccessDenied, i18n("Username or password is wrong."));
    });
}

QFuture<VideoResult> InvidiousApi::requestVideo(const QString &videoId)
{
    return get<VideoResult>(QNetworkRequest(videoUrl(videoId)), [=](QNetworkReply *reply) -> VideoResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            return Video::fromJson(doc);
        }
        return invalidJsonError();
    });
}

QFuture<SearchListResult> InvidiousApi::requestSearchResults(const SearchParameters &parameters, Paginator *paginator)
{
    QHash<QString, QString> searchParameters = parameters.toQueryParameters();

    // Invidious uses pages for all of its video results
    if (paginator != nullptr) {
        paginator->setType(Paginator::Type::Page);
        paginator->m_maxPage = std::numeric_limits<qsizetype>::max();
        searchParameters.insert(QStringLiteral("page"), QString::number(paginator->m_page + 1));
    }

    auto url = videoListUrl(Search, QStringLiteral(""), searchParameters);
    auto request = QNetworkRequest(url);

    return get<SearchListResult>(std::move(request), [=](QNetworkReply *reply) -> SearchListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto obj = doc.object();

            QList<SearchResult> results;
            for (auto value : doc.array()) {
                if (value.isObject()) {
                    results << SearchResult::fromJson(value.toObject());
                }
            }

            return results;
        }
        return invalidJsonError();
    });
}

QFuture<VideoListResult> InvidiousApi::requestFeed(Paginator *paginator)
{
    return requestVideoList(Feed, QStringLiteral(""), {}, paginator);
}

QFuture<VideoListResult> InvidiousApi::requestTop()
{
    return requestVideoList(Top);
}

QFuture<VideoListResult> InvidiousApi::requestTrending(TrendingTopic topic, Paginator *paginator)
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
    return requestVideoList(Trending, QStringLiteral(""), parameters, paginator);
}

QFuture<VideoListResult> InvidiousApi::requestChannel(const QString &query, Paginator *paginator)
{
    return requestVideoList(Channel, query, {}, paginator);
}

QFuture<SubscriptionsResult> InvidiousApi::requestSubscriptions()
{
    return get<SubscriptionsResult>(authenticatedNetworkRequest(subscriptionsUrl()), [=](QNetworkReply *reply) -> SubscriptionsResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            auto array = doc.array();

            QList<QString> subscriptions;
            std::transform(array.cbegin(), array.cend(), std::back_inserter(subscriptions), [](const QJsonValue &val) {
                return val.toObject().value(QStringLiteral("authorId")).toString();
            });
            return subscriptions;
        }
        return invalidJsonError();
    });
}

QFuture<Result> InvidiousApi::subscribeToChannel(const QString &channel)
{
    return post<Result>(authenticatedNetworkRequest(subscribeUrl(channel)), {}, checkIsReplyOk);
}

QFuture<Result> InvidiousApi::unsubscribeFromChannel(const QString &channel)
{
    return deleteResource<Result>(authenticatedNetworkRequest(subscribeUrl(channel)), checkIsReplyOk);
}

QFuture<HistoryResult> InvidiousApi::requestHistory(Paginator *paginator)
{
    paginator->setType(Paginator::Type::Page);
    paginator->m_maxPage = std::numeric_limits<qsizetype>::max();

    QUrlQuery parameters;
    parameters.addQueryItem(QStringLiteral("page"), QString::number(paginator->m_page + 1));

    QUrl url = apiUrl(API_HISTORY);
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
    return post<Result>(authenticatedNetworkRequest(apiUrl(API_HISTORY % u'/' % videoId)), {}, checkIsReplyOk);
}

QFuture<Result> InvidiousApi::markUnwatched(const QString &videoId)
{
    return deleteResource<Result>(authenticatedNetworkRequest(apiUrl(API_HISTORY % u'/' % videoId)), checkIsReplyOk);
}

QFuture<CommentsResult> InvidiousApi::requestComments(const QString &videoId, Paginator *paginator)
{
    paginator->setType(Paginator::Type::Continuation);

    QUrlQuery parameters;
    if (!paginator->m_continuation.isEmpty()) {
        parameters.addQueryItem(QStringLiteral("continuation"), paginator->m_continuation);
    }

    QUrl url = apiUrl(API_COMMENTS % u'/' % videoId);
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

            paginator->m_continuation = doc[u"continuation"].toString();

            return comments;
        }
        return invalidJsonError();
    });
}

QFuture<PlaylistsResult> InvidiousApi::requestPlaylists()
{
    QUrl url = apiUrl(API_LIST_PLAYLISTS);

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
    QUrl url = apiUrl(API_PREFERENCES);

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
    QUrl url = apiUrl(API_PREFERENCES);

    return post<Result>(authenticatedNetworkRequest(std::move(url)), QJsonDocument(preferences.toJson()).toJson(), checkIsReplyOk);
}

QFuture<VideoListResult> InvidiousApi::requestPlaylist(const QString &plid)
{
    QUrl url = apiUrl(API_PLAYLISTS % u'/' % plid);

    return get<VideoListResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> VideoListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto obj = doc.object();

            return VideoBasicInfo::fromJson(obj.value("videos"_L1).toArray());
        }
        return invalidJsonError();
    });
}

QFuture<ChannelResult> InvidiousApi::requestChannelInfo(const QString &queryd)
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

QFuture<PlaylistsResult> InvidiousApi::requestChannelPlaylists(const QString &channelId)
{
    QUrl url = apiUrl(API_CHANNEL_PLAYLISTS % u'/' % channelId);

    return get<PlaylistsResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> PlaylistsResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto array = doc["playlists"_L1].toArray();

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

QFuture<Result> InvidiousApi::addVideoToPlaylist(const QString &plid, const QString &videoId)
{
    QUrl url = apiUrl(API_LIST_PLAYLISTS % u'/' % plid % u"/videos");

    QJsonObject requestObj;
    requestObj["videoId"_L1] = videoId;

    QNetworkRequest request = authenticatedNetworkRequest(std::move(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/json"));

    return post<Result>(std::move(request), QJsonDocument(requestObj).toJson(QJsonDocument::Compact), checkIsReplyOk);
}

QFuture<Result> InvidiousApi::removeVideoFromPlaylist(const QString &plid, const QString &indexId)
{
    QUrl url = apiUrl(API_LIST_PLAYLISTS % u'/' % plid % u"/videos/" % indexId);

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

QFuture<VideoListResult>
InvidiousApi::requestVideoList(VideoListType queryType, const QString &urlExtension, const QHash<QString, QString> &parameters, Paginator *paginator)
{
    // TODO: we should really make the parameter copy, instead of doing it here?
    QHash<QString, QString> finalParameters = parameters;

    // Invidious uses pages for all of its video results
    if (paginator != nullptr) {
        paginator->setType(Paginator::Type::Page);

        // Invidious doesn't support paginating through the Trending section.
        if (queryType == Trending) {
            paginator->m_maxPage = 0;
        } else {
            paginator->m_maxPage = std::numeric_limits<qsizetype>::max();
            // TODO: Invidious pages start at 1. I wonder if we should bake this into the Paginator API?
            finalParameters.insert(QStringLiteral("page"), QString::number(paginator->m_page + 1));
        }
    }

    auto url = videoListUrl(queryType, urlExtension, finalParameters);
    // Feed requests require to be authenticated
    auto request = queryType == Feed ? authenticatedNetworkRequest(std::move(url)) : QNetworkRequest(url);

    return get<VideoListResult>(std::move(request), [=](QNetworkReply *reply) -> VideoListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            if (queryType == Feed) {
                const auto obj = doc.object();

                // add videos marked as notification
                auto results = VideoBasicInfo::fromJson(obj.value("notifications"_L1).toArray());

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
    if (isLoggedIn()) {
        const QList<QNetworkCookie> cookies{m_cookie.value()};
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

QUrl InvidiousApi::videoUrl(const QString &videoId) const
{
    return apiUrl(API_VIDEOS % u'/' % videoId);
}

QUrl InvidiousApi::videoListUrl(VideoListType queryType, const QString &urlExtension, const QHash<QString, QString> &parameters) const
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
    case Top:
        urlString.append(API_TOP);
        break;
    case Feed:
        urlString.append(API_FEED);
        break;
    case Channel:
        urlString.append(API_CHANNEL_VIDEOS);
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

QUrl InvidiousApi::subscriptionsUrl() const
{
    auto url = apiUrl(API_SUBSCRIPTIONS);
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("fields"), QStringLiteral("authorId"));
    url.setQuery(query);
    return url;
}

QUrl InvidiousApi::subscribeUrl(const QString &channelId) const
{
    return apiUrl(API_SUBSCRIPTIONS % u'/' % channelId);
}
