// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractapi.h"
#include "channel.h"
#include "comment.h"
#include "playlist.h"
#include "preferences.h"
#include "searchparameters.h"
#include "video.h"

#include <QFuture>
#include <QHash>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QObject>

#include <variant>

class QNetworkAccessManager;

namespace QInvidious
{

class PeerTubeApi : public AbstractApi
{
    Q_OBJECT

public:
    explicit PeerTubeApi(QNetworkAccessManager *netManager, QObject *parent = nullptr);

    bool isLoggedIn() const override;
    bool canLogIn() const override;
    void loadCredentials(const QString &prefix) override;
    void saveCredentials(const QString &prefix) override;
    void wipeCredentials(const QString &prefix) override;

    void prepareLogIn() override;
    QFuture<LogInResult> logIn(const QString &username, const QString &password) override;
    QFuture<VideoResult> requestVideo(const QString &videoId) override;
    QFuture<SearchListResult> requestSearchResults(const SearchParameters &parameters, Paginator *paginator = nullptr) override;
    QFuture<VideoListResult> requestFeed(Paginator *paginator = nullptr) override;
    QFuture<VideoListResult> requestTop() override;
    QFuture<VideoListResult> requestTrending(TrendingTopic = Main, Paginator *paginator = nullptr) override;
    QFuture<VideoListResult> requestChannel(const QString &query, Paginator *paginator = nullptr) override;
    QFuture<SubscriptionsResult> requestSubscriptions() override;
    QFuture<Result> subscribeToChannel(const QString &channel) override;
    QFuture<Result> unsubscribeFromChannel(const QString &channel) override;
    QFuture<HistoryResult> requestHistory(Paginator *paginator = nullptr) override;
    QFuture<Result> markWatched(const QString &videoId) override;
    QFuture<Result> markUnwatched(const QString &videoId) override;
    QFuture<CommentsResult> requestComments(const QString &videoId, Paginator *paginator = nullptr) override;
    QFuture<PlaylistsResult> requestPlaylists() override;
    QFuture<PreferencesResult> requestPreferences() override;
    QFuture<Result> setPreferences(const QInvidious::Preferences &preferences) override;
    QFuture<VideoListResult> requestPlaylist(const QString &plid) override;
    QFuture<ChannelResult> requestChannelInfo(const QString &queryd) override;
    QFuture<PlaylistsResult> requestChannelPlaylists(const QString &channelId) override;
    QFuture<Result> addVideoToPlaylist(const QString &plid, const QString &videoId) override;
    QFuture<Result> removeVideoFromPlaylist(const QString &plid, const QString &indexId) override;

private:
    enum VideoListType { Search, Trending, Top, Feed, Channel };

    static Error invalidJsonError();
    static Result checkIsReplyOk(QNetworkReply *reply);

    bool supportsFeature(SupportedFeature feature) override;

    QFuture<VideoListResult>
    requestVideoList(VideoListType queryType, const QString &urlExtension = {}, const QHash<QString, QString> &parameters = {}, Paginator *paginator = nullptr);
    QNetworkRequest authenticatedNetworkRequest(QUrl url);
    QUrlQuery genericUrlQuery() const;
    QUrl videoUrl(const QString &videoId) const;
    QUrl videoListUrl(VideoListType queryType, const QString &urlExtension = {}, const QHash<QString, QString> &parameters = {}) const;
    QUrl subscribeUrl(const QString &channelId) const;
    void fixupVideoThumbnails(QList<VideoBasicInfo> &list) const;
    void fixupVideoThumbnail(VideoBasicInfo &video) const;
    void fixupChannel(QInvidious::Channel &channel);
    void fixupVideo(QInvidious::Video &video);
    void fixupComment(QInvidious::Comment &comment);

    QString m_clientId;
    QString m_clientSecret;
    QString m_accessToken;
    QString m_refreshToken;
};

}
