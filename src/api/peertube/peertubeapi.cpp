// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "peertubeapi.h"

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringBuilder>
#include <QUrl>
#include <QUrlQuery>

#include <KLocalizedString>

const QString API_VIDEOS = QStringLiteral("/api/v1/videos");
const QString API_CHANNEL = QStringLiteral("/api/v1/video-channels");

using namespace QInvidious;
using namespace Qt::StringLiterals;

PeerTubeApi::PeerTubeApi(QNetworkAccessManager *netManager, QObject *parent)
    : AbstractApi(netManager, parent)
{
}

QFuture<LogInResult> PeerTubeApi::logIn(QStringView username, QStringView password)
{
    QUrlQuery params;
    params.addQueryItem(QStringLiteral("email"), QString::fromUtf8(QUrl::toPercentEncoding(username.toString())));
    params.addQueryItem(QStringLiteral("password"), QString::fromUtf8(QUrl::toPercentEncoding(password.toString())));
    params.addQueryItem(QStringLiteral("action"), QStringLiteral("signin"));

    QNetworkRequest request(logInUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/x-www-form-urlencoded"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::RedirectPolicy::ManualRedirectPolicy);

    return post<LogInResult>(std::move(request), params.toString().toUtf8(), [=](QNetworkReply *reply) -> LogInResult {
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

QFuture<VideoResult> PeerTubeApi::requestVideo(QStringView videoId)
{
    return get<VideoResult>(QNetworkRequest(videoUrl(videoId)), [=](QNetworkReply *reply) -> VideoResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            return Video::fromJson(doc);
        }
        return invalidJsonError();
    });
}

QString PeerTubeApi::resolveVideoUrl(QStringView videoId)
{
    return QStringLiteral("https://%1/videos/watch/%2").arg(m_apiHost, videoId);
}

QFuture<VideoListResult> PeerTubeApi::requestSearchResults(const SearchParameters &parameters)
{
    return requestVideoList(Search, QStringLiteral(""), parameters.toQueryParameters());
}

QFuture<VideoListResult> PeerTubeApi::requestFeed(qint32 page)
{
    QHash<QString, QString> parameters;
    parameters.insert(QStringLiteral("page"), QString::number(page));

    return requestVideoList(Feed, QStringLiteral(""), parameters);
}

QFuture<VideoListResult> PeerTubeApi::requestTop()
{
    return requestVideoList(Top);
}

QFuture<VideoListResult> PeerTubeApi::requestTrending(TrendingTopic topic)
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

QFuture<VideoListResult> PeerTubeApi::requestChannel(QStringView query, qint32 page)
{
    QHash<QString, QString> parameters;
    parameters.insert(QStringLiteral("page"), QString::number(page));
    return requestVideoList(Channel, query.toString(), parameters);
}

QFuture<SubscriptionsResult> PeerTubeApi::requestSubscriptions()
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

QFuture<Result> PeerTubeApi::subscribeToChannel(QStringView channel)
{
    return post<Result>(authenticatedNetworkRequest(subscribeUrl(channel)), {}, checkIsReplyOk);
}

QFuture<Result> PeerTubeApi::unsubscribeFromChannel(QStringView channel)
{
    return deleteResource<Result>(authenticatedNetworkRequest(subscribeUrl(channel)), checkIsReplyOk);
}

QFuture<HistoryResult> PeerTubeApi::requestHistory(qint32 page)
{
    return {};
}

QFuture<Result> PeerTubeApi::markWatched(const QString &videoId)
{
    return {};
}

QFuture<Result> PeerTubeApi::markUnwatched(const QString &videoId)
{
    return {};
}

QFuture<CommentsResult> PeerTubeApi::requestComments(const QString &videoId, const QString &continuation)
{
    QUrl url = apiUrl(API_VIDEOS % u'/' % videoId % u"/comment-threads");

    return get<CommentsResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> CommentsResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            const auto array = doc[u"data"].toArray();

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
    return {};
}

QFuture<VideoListResult> PeerTubeApi::requestPlaylist(const QString &plid)
{
    return {};
}

QFuture<ChannelResult> PeerTubeApi::requestChannelInfo(QStringView queryd)
{
    QUrl url = apiUrl(API_CHANNEL % u'/' % queryd);

    return get<ChannelResult>(authenticatedNetworkRequest(std::move(url)), [=](QNetworkReply *reply) -> ChannelResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            QInvidious::Channel channel;
            Channel::fromJson(doc.object(), channel);
            fixupChannel(channel);
            return channel;
        }
        return invalidJsonError();
    });
}

QFuture<Result> PeerTubeApi::addVideoToPlaylist(const QString &plid, const QString &videoId)
{
    return {};
}

QFuture<Result> PeerTubeApi::removeVideoFromPlaylist(const QString &plid, const QString &indexId)
{
    return {};
}

Error PeerTubeApi::invalidJsonError()
{
    return std::pair(QNetworkReply::InternalServerError, i18n("Server returned no valid JSON."));
}

Result PeerTubeApi::checkIsReplyOk(QNetworkReply *reply)
{
    auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status >= 200 && status < 300) {
        return Success();
    }
    return std::pair(QNetworkReply::InternalServerError, i18n("Server returned the status code %1", QString::number(status)));
}

QFuture<VideoListResult> PeerTubeApi::requestVideoList(VideoListType queryType, const QString &urlExtension, const QHash<QString, QString> &parameters)
{
    auto url = videoListUrl(queryType, urlExtension, parameters);
    // Feed requests require to be authenticated
    auto request = queryType == Feed ? authenticatedNetworkRequest(std::move(url)) : QNetworkRequest(url);

    return get<VideoListResult>(std::move(request), [=](QNetworkReply *reply) -> VideoListResult {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            if (queryType == Feed) {
                const auto obj = doc.object();

                auto results = VideoBasicInfo::fromJson(obj.value("data"_L1).toArray());
                fixupVideoThumbnails(results);
                return results;
            } else if (queryType == Channel) {
                const auto obj = doc.object();

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
    if (!m_credentials.isAnonymous()) {
        const QList<QNetworkCookie> cookies{m_credentials.cookie().value()};
        request.setHeader(QNetworkRequest::CookieHeader, QVariant::fromValue(cookies));
    }
    // some invidious instances redirect some calls using reverse proxies
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    return request;
}

QUrlQuery PeerTubeApi::genericUrlQuery() const
{
    return {};
}

QUrl PeerTubeApi::logInUrl() const
{
    return {};
}

QUrl PeerTubeApi::videoUrl(QStringView videoId) const
{
    return apiUrl(API_VIDEOS % u'/' % videoId);
}

QUrl PeerTubeApi::videoListUrl(VideoListType queryType, const QString &urlExtension, const QHash<QString, QString> &parameters) const
{
    // TODO: implement other query types
    QString urlString = API_VIDEOS;
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

QUrl PeerTubeApi::subscriptionsUrl() const
{
    return {};
}

QUrl PeerTubeApi::subscribeUrl(QStringView channelId) const
{
    return {};
}

void PeerTubeApi::fixupVideoThumbnails(QList<VideoBasicInfo> &list) const
{
    // PeerTube gives us relative URLs for thumbnails (why?) so we need to attach the api instance
    for (auto &video : list) {
        auto newThumbnails = video.videoThumbnails();
        for (auto &thumbnail : newThumbnails) {
            thumbnail.setUrl(QUrl(QStringLiteral("https://%1/%2").arg(m_apiHost, thumbnail.url().path())));
        }
        video.setVideoThumbnails(newThumbnails);
    }
}

void PeerTubeApi::fixupChannel(QInvidious::Channel &channel)
{
    // PeerTube gives us relative URLs for avatar/banner
    channel.setAvatar(QStringLiteral("https://%1/%2").arg(m_apiHost, channel.avatar()));
    channel.setBanner(QStringLiteral("https://%1/%2").arg(m_apiHost, channel.banner()));
}
