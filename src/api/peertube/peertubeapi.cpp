// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "peertubeapi.h"

#include <KLocalizedString>

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringBuilder>
#include <QUrl>
#include <QUrlQuery>

const QString API_VIDEOS = QStringLiteral("/api/v1/videos");
const QString API_CHANNEL = QStringLiteral("/api/v1/video-channels");
const QString API_SEARCH_VIDEOS = QStringLiteral("/api/v1/search/videos");
const QString API_SEARCH_VIDEO_CHANNELS = QStringLiteral("/api/v1/search/video-channels");
const QString API_SEARCH_VIDEO_PLAYLISTS = QStringLiteral("/api/v1/search/video-playlists");
const QString API_OAUTH_TOKEN = QStringLiteral("/api/v1/oauth-clients/local");
const QString API_LOGIN_TOKEN = QStringLiteral("/api/v1/users/token");
const QString API_SUBSCRIPTION_VIDEOS = QStringLiteral("/api/v1/users/me/subscriptions/videos");
const QString API_SUBSCRIPTIONS = QStringLiteral("/api/v1/users/me/subscriptions");

const QString CLIENT_ID_KEY = QStringLiteral("client-id");
const QString CLIENT_SECRET_KEY = QStringLiteral("client-secret");
const QString ACCESS_TOKEN_KEY = QStringLiteral("access-token");
const QString REFRESH_TOKEN_KEY = QStringLiteral("refresh-token");

using namespace QInvidious;
using namespace Qt::StringLiterals;

PeerTubeApi::PeerTubeApi(QNetworkAccessManager *netManager, QObject *parent)
    : AbstractApi(netManager, parent)
{
}

bool PeerTubeApi::isLoggedIn() const
{
    return !m_clientId.isEmpty() && !m_clientSecret.isEmpty() && !m_accessToken.isEmpty() && !m_refreshToken.isEmpty();
}

bool PeerTubeApi::canLogIn() const
{
    return !m_clientId.isEmpty() && !m_clientSecret.isEmpty();
}

void PeerTubeApi::loadCredentials(const QString &prefix)
{
    if (auto clientId = getKeychainValue(prefix, CLIENT_ID_KEY)) {
        m_clientId = *clientId;
    }
    if (auto clientSecret = getKeychainValue(prefix, CLIENT_SECRET_KEY)) {
        m_clientSecret = *clientSecret;
    }
    if (auto accessToken = getKeychainValue(prefix, ACCESS_TOKEN_KEY)) {
        m_accessToken = *accessToken;
    }
    if (auto refreshToken = getKeychainValue(prefix, REFRESH_TOKEN_KEY)) {
        m_refreshToken = *refreshToken;
    }
    Q_EMIT credentialsChanged();
}

void PeerTubeApi::saveCredentials(const QString &prefix)
{
    if (!m_clientId.isEmpty()) {
        setKeychainValue(prefix, CLIENT_ID_KEY, m_clientId);
    }
    if (!m_clientSecret.isEmpty()) {
        setKeychainValue(prefix, CLIENT_SECRET_KEY, m_clientSecret);
    }
    if (!m_accessToken.isEmpty()) {
        setKeychainValue(prefix, ACCESS_TOKEN_KEY, m_accessToken);
    }
    if (!m_refreshToken.isEmpty()) {
        setKeychainValue(prefix, REFRESH_TOKEN_KEY, m_refreshToken);
    }
}

void PeerTubeApi::wipeCredentials(const QString &prefix)
{
    wipeKeychainValue(prefix, CLIENT_ID_KEY);
    wipeKeychainValue(prefix, CLIENT_SECRET_KEY);
    wipeKeychainValue(prefix, ACCESS_TOKEN_KEY);
    wipeKeychainValue(prefix, REFRESH_TOKEN_KEY);

    m_clientId.clear();
    m_clientSecret.clear();
    m_accessToken.clear();
    m_refreshToken.clear();

    Q_EMIT credentialsChanged();
}

bool PeerTubeApi::supportsFeature(AbstractApi::SupportedFeature feature)
{
    switch (feature) {
    case PopularPage:
    case TrendingCategories:
        return false;
    }

    return false;
}

void PeerTubeApi::prepareLogIn()
{
    QNetworkRequest request(apiUrl(API_OAUTH_TOKEN));
    auto reply = m_netManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        if (reply->error() == QNetworkReply::NoError) {
            const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            auto obj = doc.object();
            if (obj.contains("client_id"_L1)) {
                m_clientId = obj["client_id"_L1].toString();
            }
            if (obj.contains("client_secret"_L1)) {
                m_clientSecret = obj["client_secret"_L1].toString();
            }
            Q_EMIT canLogInChanged();
        }
    });
}

QFuture<LogInResult> PeerTubeApi::logIn(const QString &username, const QString &password)
{
    QUrlQuery params;
    params.addQueryItem(QStringLiteral("username"), QString::fromUtf8(QUrl::toPercentEncoding(username)));
    params.addQueryItem(QStringLiteral("password"), QString::fromUtf8(QUrl::toPercentEncoding(password)));
    params.addQueryItem(QStringLiteral("grant_type"), QStringLiteral("password"));
    params.addQueryItem(QStringLiteral("client_id"), m_clientId);
    params.addQueryItem(QStringLiteral("client_secret"), m_clientSecret);

    QNetworkRequest request(apiUrl(API_LOGIN_TOKEN));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/x-www-form-urlencoded"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::RedirectPolicy::ManualRedirectPolicy);

    return post<LogInResult>(std::move(request), params.toString().toUtf8(), [this](QNetworkReply *reply) -> LogInResult {
        if (reply->error() == QNetworkReply::NoError) {
            const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            auto obj = doc.object();

            m_accessToken = obj["access_token"_L1].toString();
            m_refreshToken = obj["refresh_token"_L1].toString();
            Q_EMIT credentialsChanged();

            return std::nullopt;
        } else {
            return std::pair(reply->error(), reply->errorString());
        }
    });
}

QFuture<VideoResult> PeerTubeApi::requestVideo(const QString &videoId)
{
    // This needs to be authenticated depending on where the video id is from (like subscriptions)
    return get<VideoResult>(authenticatedNetworkRequest(videoUrl(videoId)), [this](QNetworkReply *reply) -> VideoResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            auto video = Video::fromJson(doc);
            fixupVideo(video);
            return video;
        }
        return invalidJsonError();
    });
}

QFuture<SearchListResult> PeerTubeApi::requestSearchResults(const SearchParameters &parameters, Paginator *paginator)
{
    QHash<QString, QString> searchParameters = {{u"search"_s, parameters.query()}};

    if (paginator != nullptr) {
        paginator->setType(Paginator::Type::StartIndex);
        searchParameters[u"start"_s] = QString::number(paginator->m_startIndex);
    }

    if (parameters.sortBy() == SearchParameters::SortBy::UploadDate) {
        searchParameters[u"sort"_s] = u"-createdAt"_s;
    }
    auto url = videoListUrl(Search, QString(), searchParameters);

    auto request = QNetworkRequest(url);

    return get<SearchListResult>(std::move(request), [this, paginator](QNetworkReply *reply) -> SearchListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto obj = doc.object();

            QList<SearchResult> results;
            const auto resultsJson = obj["data"_L1].toArray();
            for (auto value : resultsJson) {
                if (value.isObject()) {
                    auto result = SearchResult::fromJson(value.toObject());
                    auto video = result.video();
                    fixupVideoThumbnail(video);
                    result.setVideo(video);
                    results << result;
                }
            }

            paginator->m_total = obj["total"_L1].toInt();
            paginator->m_count = resultsJson.size();

            return results;
        }
        return invalidJsonError();
    });
}

QFuture<VideoListResult> PeerTubeApi::requestFeed(Paginator *paginator)
{
    return requestVideoList(Feed, QStringLiteral(""), {}, paginator);
}

QFuture<VideoListResult> PeerTubeApi::requestTop()
{
    return requestVideoList(Top);
}

QFuture<VideoListResult> PeerTubeApi::requestTrending(TrendingTopic topic, Paginator *paginator)
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

QFuture<VideoListResult> PeerTubeApi::requestChannel(const QString &query, Paginator *paginator)
{
    return requestVideoList(Channel, query, {}, paginator);
}

QFuture<SubscriptionsResult> PeerTubeApi::requestSubscriptions()
{
    // TODO: paginate!
    return get<SubscriptionsResult>(authenticatedNetworkRequest(apiUrl(API_SUBSCRIPTIONS)), [=](QNetworkReply *reply) -> SubscriptionsResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            auto array = doc.object()["data"_L1].toArray();

            QList<QString> subscriptions;
            std::transform(array.cbegin(), array.cend(), std::back_inserter(subscriptions), [](const QJsonValue &val) {
                return val.toObject().value(QStringLiteral("name")).toString() + QLatin1Char('@') + val.toObject().value(QStringLiteral("host")).toString();
            });
            return subscriptions;
        }
        return invalidJsonError();
    });
}

QFuture<Result> PeerTubeApi::subscribeToChannel(const QString &channel)
{
    const QJsonObject obj{{QStringLiteral("uri"), channel}};
    const QJsonDocument doc = QJsonDocument(obj);
    return post<Result>(authenticatedNetworkRequest(apiUrl(API_SUBSCRIPTIONS)), doc.toJson(), checkIsReplyOk);
}

QFuture<Result> PeerTubeApi::unsubscribeFromChannel(const QString &channel)
{
    return deleteResource<Result>(authenticatedNetworkRequest(apiUrl(API_SUBSCRIPTIONS + u"/" + channel)), checkIsReplyOk);
}

QFuture<HistoryResult> PeerTubeApi::requestHistory(Paginator *paginator)
{
    Q_UNUSED(paginator)
    return {};
}

QFuture<Result> PeerTubeApi::markWatched(const QString &videoId)
{
    Q_UNUSED(videoId)
    return {};
}

QFuture<Result> PeerTubeApi::markUnwatched(const QString &videoId)
{
    Q_UNUSED(videoId)
    return {};
}

QFuture<CommentsResult> PeerTubeApi::requestComments(const QString &videoId, Paginator *paginator)
{
    Q_UNUSED(paginator)

    QUrl url = apiUrl(API_VIDEOS % u'/' % videoId % u"/comment-threads");

    return get<CommentsResult>(authenticatedNetworkRequest(std::move(url)), [this](QNetworkReply *reply) -> CommentsResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto array = doc[u"data"].toArray();

            QList<Comment> comments;
            std::transform(array.cbegin(), array.cend(), std::back_inserter(comments), [this](const QJsonValue &val) {
                Comment comment;
                Comment::fromJson(val.toObject(), comment);
                fixupComment(comment);
                return comment;
            });
            return comments;
        }
        return invalidJsonError();
    });
}

QFuture<PlaylistsResult> PeerTubeApi::requestPlaylists()
{
    return {};
}

QFuture<PreferencesResult> PeerTubeApi::requestPreferences()
{
    return {};
}

QFuture<Result> PeerTubeApi::setPreferences(const QInvidious::Preferences &preferences)
{
    Q_UNUSED(preferences)
    return {};
}

QFuture<VideoListResult> PeerTubeApi::requestPlaylist(const QString &plid)
{
    Q_UNUSED(plid)
    return {};
}

QFuture<ChannelResult> PeerTubeApi::requestChannelInfo(const QString &queryd)
{
    QUrl url = apiUrl(API_CHANNEL % u'/' % queryd);

    return get<ChannelResult>(authenticatedNetworkRequest(std::move(url)), [this](QNetworkReply *reply) -> ChannelResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            QInvidious::Channel channel;
            Channel::fromJson(doc.object(), channel);
            fixupChannel(channel);
            return channel;
        }
        return invalidJsonError();
    });
}

QFuture<PlaylistsResult> PeerTubeApi::requestChannelPlaylists(const QString &channelId)
{
    // TODO: peertube stub
    Q_UNUSED(channelId)
    return {};
}

QFuture<Result> PeerTubeApi::addVideoToPlaylist(const QString &plid, const QString &videoId)
{
    Q_UNUSED(plid)
    Q_UNUSED(videoId)
    return {};
}

QFuture<Result> PeerTubeApi::removeVideoFromPlaylist(const QString &plid, const QString &indexId)
{
    Q_UNUSED(plid)
    Q_UNUSED(indexId)
    return {};
}

Error PeerTubeApi::invalidJsonError()
{
    return {QNetworkReply::InternalServerError, i18n("Server returned no valid JSON.")};
}

Result PeerTubeApi::checkIsReplyOk(QNetworkReply *reply)
{
    auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status >= 200 && status < 300) {
        return Success();
    }
    return std::pair(QNetworkReply::InternalServerError, i18n("Server returned the status code %1", QString::number(status)));
}

QFuture<VideoListResult>
PeerTubeApi::requestVideoList(VideoListType queryType, const QString &urlExtension, const QHash<QString, QString> &parameters, Paginator *paginator)
{
    // TODO: we should really make the parameter copy, instead of doing it here?
    QHash<QString, QString> finalParameters = parameters;

    // PeerTube uses the startIndex for paging video requests
    if (paginator != nullptr) {
        paginator->setType(Paginator::Type::StartIndex);
        finalParameters.insert(QStringLiteral("start"), QString::number(paginator->m_startIndex));
    }

    auto url = videoListUrl(queryType, urlExtension, finalParameters);

    // PeerTube requests should always be authenticated
    auto request = authenticatedNetworkRequest(std::move(url));

    return get<VideoListResult>(std::move(request), [this, paginator, queryType](QNetworkReply *reply) -> VideoListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            auto obj = doc.object();
            if (paginator != nullptr && obj.contains("total"_L1)) {
                paginator->m_total = obj["total"_L1].toInt();
            }

            if (paginator != nullptr && obj.contains("data"_L1) && obj["data"_L1].isArray()) {
                paginator->m_count = obj["data"_L1].toArray().size();
            }

            if (queryType == Feed) {
                auto results = VideoBasicInfo::fromJson(obj.value("data"_L1).toArray());
                fixupVideoThumbnails(results);
                return results;
            } else if (queryType == Channel) {
                auto results = VideoBasicInfo::fromJson(obj.value("data"_L1).toArray());
                fixupVideoThumbnails(results);
                return results;
            } else {
                QList<VideoBasicInfo> results;
                for (auto value : doc["data"_L1].toArray()) {
                    if (value.isObject()) {
                        results << VideoBasicInfo::fromJson(value.toObject());
                    }
                }

                fixupVideoThumbnails(results);
                return results;
            }
        }
        return invalidJsonError();
    });
}

QNetworkRequest PeerTubeApi::authenticatedNetworkRequest(QUrl &&url)
{
    QNetworkRequest request(url);
    if (isLoggedIn()) {
        request.setRawHeader(QByteArrayLiteral("Authorization"), QStringLiteral("Bearer %1").arg(m_accessToken).toUtf8());
    }
    return request;
}

QUrlQuery PeerTubeApi::genericUrlQuery() const
{
    return {};
}

QUrl PeerTubeApi::videoUrl(const QString &videoId) const
{
    return apiUrl(API_VIDEOS % u'/' % videoId);
}

QUrl PeerTubeApi::videoListUrl(VideoListType queryType, const QString &urlExtension, const QHash<QString, QString> &parameters) const
{
    QString urlString;
    switch (queryType) {
    case Feed:
        urlString = API_SUBSCRIPTION_VIDEOS;
        break;
    case Search:
        urlString = API_SEARCH_VIDEOS;
        break;
    case Channel:
        urlString = API_CHANNEL;
        break;
    default:
        // TODO: implement other query types
        urlString = API_VIDEOS;
        break;
    }

    auto query = genericUrlQuery();

    if (!urlExtension.isEmpty()) {
        urlString.append(QStringLiteral("/"));
        urlString.append(urlExtension);
        if (queryType == Channel) {
            urlString.append(QStringLiteral("/videos"));
        }
    }

    for (QHash<QString, QString>::const_iterator parameter = parameters.begin(); parameter != parameters.end(); ++parameter) {
        query.addQueryItem(parameter.key(), parameter.value());
    }

    QUrl url = apiUrl(urlString);
    url.setQuery(query);
    return url;
}

QUrl PeerTubeApi::subscribeUrl(const QString &channelId) const
{
    Q_UNUSED(channelId)
    return {};
}

void PeerTubeApi::fixupVideoThumbnails(QList<VideoBasicInfo> &list) const
{
    for (auto &video : list) {
        fixupVideoThumbnail(video);
    }
}

void PeerTubeApi::fixupVideoThumbnail(VideoBasicInfo &video) const
{
    auto newThumbnails = video.videoThumbnails();
    for (auto &thumbnail : newThumbnails) {
        // If the URL is relative, attach the server URL
        if (thumbnail.url().isRelative()) {
            thumbnail.setUrl(QUrl(QStringLiteral("https://%1/%2").arg(m_apiHost, thumbnail.url().path())));
        }
    }
    video.setVideoThumbnails(newThumbnails);
}

void PeerTubeApi::fixupChannel(QInvidious::Channel &channel)
{
    // PeerTube gives us relative URLs for avatar/banner
    channel.setAvatar(QStringLiteral("https://%1/%2").arg(m_apiHost, channel.avatar()));
    channel.setBanner(QStringLiteral("https://%1/%2").arg(m_apiHost, channel.banner()));
}

void PeerTubeApi::fixupVideo(Video &video)
{
    auto newThumbnails = video.authorThumbnails();
    for (auto &thumbnail : newThumbnails) {
        // If the URL is relative, attach the server URL
        if (thumbnail.url().isRelative()) {
            thumbnail.setUrl(QUrl(QStringLiteral("https://%1/%2").arg(m_apiHost, thumbnail.url().path())));
        }
    }
    video.setAuthorThumbnails(newThumbnails);
}

void PeerTubeApi::fixupComment(Comment &comment)
{
    comment.setAuthorAvatar(QStringLiteral("https://%1/%2").arg(m_apiHost, comment.authorAvatar()));
}
