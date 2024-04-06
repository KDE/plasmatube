// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "channel.h"
#include "comment.h"
#include "credentials.h"
#include "playlist.h"
#include "preferences.h"
#include "searchparameters.h"
#include "searchresult.h"
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

struct Comments {
    QList<Comment> comments;
    QString continuation;
};

using Error = std::pair<QNetworkReply::NetworkError, QString>;
using Success = std::monostate;
using LogInResult = std::variant<Credentials, Error>;
using VideoResult = std::variant<Video, Error>;
using VideoListResult = std::variant<QList<VideoBasicInfo>, Error>;
using SubscriptionsResult = std::variant<QList<QString>, Error>;
using Result = std::variant<Success, Error>;
using HistoryResult = std::variant<QList<QString>, Error>;
using CommentsResult = std::variant<Comments, Error>;
using PlaylistsResult = std::variant<QList<Playlist>, Error>;
using PreferencesResult = std::variant<Preferences, Error>;
using ChannelResult = std::variant<QInvidious::Channel, Error>;
using SearchListResult = std::variant<QList<SearchResult>, Error>;

enum TrendingTopic : quint8 { Main, Music, Gaming, Movies, News };

class AbstractApi : public QObject
{
    Q_OBJECT

public:
    explicit AbstractApi(QNetworkAccessManager *netManager, QObject *parent = nullptr);

    enum SupportedFeature { PopularPage, TrendingCategories };
    Q_ENUM(SupportedFeature);

    virtual bool supportsFeature(SupportedFeature feature) = 0;

    QString region() const;
    void setRegion(const QString &region);

    QString language() const;
    void setLanguage(const QString &language);

    Credentials credentials() const;
    void setCredentials(const Credentials &credentials);

    QNetworkAccessManager *net() const;

    void setApiHost(const QString &host);
    QString apiHost() const;

    virtual QFuture<LogInResult> logIn(QStringView username, QStringView password) = 0;
    virtual QFuture<VideoResult> requestVideo(QStringView videoId) = 0;
    virtual QString resolveVideoUrl(QStringView videoId) = 0;
    virtual QFuture<SearchListResult> requestSearchResults(const SearchParameters &parameters) = 0;
    virtual QFuture<VideoListResult> requestFeed(qint32 page = 1) = 0;
    virtual QFuture<VideoListResult> requestTop() = 0;
    virtual QFuture<VideoListResult> requestTrending(TrendingTopic = Main) = 0;
    virtual QFuture<VideoListResult> requestChannel(QStringView query, qint32 page = 1) = 0;
    virtual QFuture<SubscriptionsResult> requestSubscriptions() = 0;
    virtual QFuture<Result> subscribeToChannel(QStringView channel) = 0;
    virtual QFuture<Result> unsubscribeFromChannel(QStringView channel) = 0;
    virtual QFuture<HistoryResult> requestHistory(qint32 page = 1) = 0;
    virtual QFuture<Result> markWatched(const QString &videoId) = 0;
    virtual QFuture<Result> markUnwatched(const QString &videoId) = 0;
    virtual QFuture<CommentsResult> requestComments(const QString &videoId, const QString &continuation = {}) = 0;
    virtual QFuture<PlaylistsResult> requestPlaylists() = 0;
    virtual QFuture<PreferencesResult> requestPreferences() = 0;
    virtual QFuture<Result> setPreferences(const QInvidious::Preferences &preferences) = 0;
    virtual QFuture<VideoListResult> requestPlaylist(const QString &plid) = 0;
    virtual QFuture<ChannelResult> requestChannelInfo(QStringView queryd) = 0;
    virtual QFuture<PlaylistsResult> requestChannelPlaylists(const QString &channelId) = 0;
    virtual QFuture<Result> addVideoToPlaylist(const QString &plid, const QString &videoId) = 0;
    virtual QFuture<Result> removeVideoFromPlaylist(const QString &plid, const QString &indexId) = 0;

Q_SIGNALS:
    void credentialsChanged();

protected:
    QUrl apiUrl(const QString &path) const;

    template<typename T>
    QFuture<T> reportResults(QNetworkReply *reply, std::function<T(QNetworkReply *)> processs)
    {
        auto interface = std::make_shared<QFutureInterface<T>>(QFutureInterfaceBase::Started);

        connect(reply, &QNetworkReply::finished, this, [=]() {
            if (!interface->isCanceled()) {
                interface->reportResult(processs(reply));
                interface->reportFinished();
            }
            reply->deleteLater();
        });
        connect(reply, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError error) {
            qWarning() << "Error for" << error << reply->url() << reply->errorString();
            interface->reportResult(std::pair(error, reply->errorString()));
            interface->reportFinished();
            reply->deleteLater();
        });

        return interface->future();
    }

    template<typename T>
    QFuture<T> get(QNetworkRequest &&request, std::function<T(QNetworkReply *)> processs)
    {
        return reportResults<T>(m_netManager->get(request), std::move(processs));
    }

    template<typename T>
    QFuture<T> post(QNetworkRequest &&request, const QByteArray &data, std::function<T(QNetworkReply *)> processs)
    {
        return reportResults<T>(m_netManager->post(request, data), std::move(processs));
    }

    template<typename T>
    QFuture<T> deleteResource(QNetworkRequest &&request, std::function<T(QNetworkReply *)> processs)
    {
        return reportResults<T>(m_netManager->deleteResource(request), std::move(processs));
    }

    QNetworkAccessManager *m_netManager;
    QString m_region;
    QString m_language;
    QString m_apiHost;
    Credentials m_credentials;
};

}
