// SPDX-FileCopyrightText: 2026 Plasmatube Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "youtubeapi.h"

#include <KLocalizedString>

#include <QNetworkAccessManager>

using namespace QInvidious;

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
    Q_UNUSED(videoId)
    return {};
}

QFuture<SearchListResult> YouTubeApi::requestSearchResults(const SearchParameters &parameters, Paginator *paginator)
{
    Q_UNUSED(parameters)
    Q_UNUSED(paginator)
    return {};
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
    return {};
}

QFuture<VideoListResult> YouTubeApi::requestChannel(const QString &query, Paginator *paginator)
{
    Q_UNUSED(query)
    Q_UNUSED(paginator)
    return {};
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
