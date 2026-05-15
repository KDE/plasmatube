// SPDX-FileCopyrightText: 2026 Plasmatube Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractapi.h"
#include "videobasicinfo.h"

#include <QJsonObject>

namespace QInvidious
{

class YouTubeApi : public AbstractApi
{
    Q_OBJECT

public:
    explicit YouTubeApi(QNetworkAccessManager *netManager, QObject *parent = nullptr);

    bool isLoggedIn() const override;
    bool canLogIn() const override;
    void loadCredentials(const QString &prefix) override;
    void saveCredentials(const QString &prefix) override;
    void wipeCredentials(const QString &prefix) override;

    QFuture<LogInResult> logIn(const QString &username, const QString &password) override;
    QFuture<VideoResult> requestVideo(const QString &videoId) override;
    QFuture<SearchListResult> requestSearchResults(const SearchParameters &parameters, Paginator *paginator = nullptr) override;
    QFuture<VideoListResult> requestFeed(Paginator *paginator = nullptr) override;
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
    QString getVideoUrl(const QString &videoId) override;

private:
    bool supportsFeature(SupportedFeature feature) override;

    template<typename T>
    QFuture<T> innertubePost(const QString &endpoint, const QJsonObject &payload, std::function<T(QNetworkReply *)> process);

    QJsonObject baseContext() const;

    static QJsonObject videoRendererToInvidious(const QJsonObject &renderer);
    static QJsonObject tileRendererToInvidious(const QJsonObject &renderer);
    static QString flattenRuns(const QJsonValue &value);
    static qint64 parseDigits(const QString &text);
    static qint64 parseDuration(const QString &text);
    static QList<QJsonObject> walkVideoItems(const QJsonObject &root);
    static QList<VideoBasicInfo> parseVideoRenderers(const QJsonObject &root);

    QJsonObject dumpVideoInfo(const QString &videoId);
    static void splitFormats(const QJsonArray &ytdlpFormats, QJsonArray &adaptive, QJsonArray &combined);

    static Error invalidJsonError();

    QHash<QString, QJsonObject> m_videoInfoCache;
};

}
