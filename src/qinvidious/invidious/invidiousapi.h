// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// std
#include <variant>
// Qt
#include <QFuture>
#include <QHash>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QObject>
// QInvidious
#include "abstractapi.h"
#include "channel.h"
#include "comment.h"
#include "credentials.h"
#include "playlist.h"
#include "preferences.h"
#include "searchparameters.h"
#include "video.h"

class QNetworkAccessManager;

namespace QInvidious
{

class InvidiousApi : public AbstractApi
{
    Q_OBJECT

public:
    explicit InvidiousApi(QNetworkAccessManager *netManager, QObject *parent = nullptr);

    QFuture<LogInResult> logIn(QStringView username, QStringView password) override;
    QFuture<VideoResult> requestVideo(QStringView videoId) override;
    QString resolveVideoUrl(QStringView videoId) override;
    QFuture<VideoListResult> requestSearchResults(const SearchParameters &parameters) override;
    QFuture<VideoListResult> requestFeed(qint32 page = 1) override;
    QFuture<VideoListResult> requestTop() override;
    QFuture<VideoListResult> requestTrending(TrendingTopic = Main) override;
    QFuture<VideoListResult> requestChannel(QStringView query, qint32 page = 1) override;
    QFuture<SubscriptionsResult> requestSubscriptions() override;
    QFuture<Result> subscribeToChannel(QStringView channel) override;
    QFuture<Result> unsubscribeFromChannel(QStringView channel) override;
    QFuture<HistoryResult> requestHistory(qint32 page = 1) override;
    QFuture<Result> markWatched(const QString &videoId) override;
    QFuture<Result> markUnwatched(const QString &videoId) override;
    QFuture<CommentsResult> requestComments(const QString &videoId, const QString &continuation = {}) override;
    QFuture<PlaylistsResult> requestPlaylists() override;
    QFuture<PreferencesResult> requestPreferences() override;
    QFuture<Result> setPreferences(const QInvidious::Preferences &preferences) override;
    QFuture<VideoListResult> requestPlaylist(const QString &plid) override;
    QFuture<ChannelResult> requestChannelInfo(QStringView queryd) override;
    QFuture<Result> addVideoToPlaylist(const QString &plid, const QString &videoId) override;
    QFuture<Result> removeVideoFromPlaylist(const QString &plid, const QString &indexId) override;

private:
    enum VideoListType { Search, Trending, Top, Feed, Channel };

    static Error invalidJsonError();
    static Result checkIsReplyOk(QNetworkReply *reply);

    QFuture<VideoListResult> requestVideoList(VideoListType queryType, const QString &urlExtension = {}, const QHash<QString, QString> &parameters = {});
    QNetworkRequest authenticatedNetworkRequest(QUrl &&url);
    QUrlQuery genericUrlQuery() const;
    QUrl logInUrl() const;
    QUrl videoUrl(QStringView videoId) const;
    QUrl videoListUrl(VideoListType queryType, const QString &urlExtension = {}, const QHash<QString, QString> &parameters = {}) const;
    QUrl subscriptionsUrl() const;
    QUrl subscribeUrl(QStringView channelId) const;
};

};
