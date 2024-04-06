// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "channel.h"
#include "comment.h"
#include "paginator.h"
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

using VideoList = QList<VideoBasicInfo>;

using Error = std::pair<QNetworkReply::NetworkError, QString>;
using Success = std::monostate;
using LogInResult = std::optional<Error>;
using VideoResult = std::variant<Video, Error>;
using VideoListResult = std::variant<VideoList, Error>;
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

    virtual bool isLoggedIn() const = 0;
    virtual bool canLogIn() const = 0;

    /**
     * @brief Loads the credentials (if any) under @p prefix in secure storage.
     */
    virtual void loadCredentials(const QString &prefix) = 0;

    // TODO: should this be public API? it seems you should be using logIn()
    /**
     * @brief Stores the credentials (if any) under @p prefix in secure storage.
     */
    virtual void saveCredentials(const QString &prefix) = 0;

    /**
     * @brief Wipe all credentials (if any) under @p prefix in secure storage.
     */
    virtual void wipeCredentials(const QString &prefix) = 0;

    void setApiHost(const QString &host);
    QString apiHost() const;

    /**
     * @brief Should be called once the login page is shown. Required by some API backends to prepare.
     */
    virtual void prepareLogIn()
    {
    }
    virtual QFuture<LogInResult> logIn(QStringView username, QStringView password) = 0;
    virtual QFuture<VideoResult> requestVideo(QStringView videoId) = 0;
    virtual QString resolveVideoUrl(QStringView videoId) = 0;
    virtual QFuture<SearchListResult> requestSearchResults(const SearchParameters &parameters) = 0;
    /**
     * @brief Request the subscription feed, only works if logged in.
     * @return Use a @c Paginator if you wish to paginate through the results, otherwise returns the first page of results.
     */
    virtual QFuture<VideoListResult> requestFeed(Paginator * = nullptr) = 0;
    virtual QFuture<VideoListResult> requestTop() = 0;
    /**
     * @brief Request the trending videos on this server, if available.
     * @note The trending topic may be ignored if this API does not support it. Check this feature by calling supportsFeature().
     * @return Use a @c Paginator if you wish to paginate through the results, otherwise returns the first page of results.
     */
    virtual QFuture<VideoListResult> requestTrending(TrendingTopic = Main, Paginator * = nullptr) = 0;
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
    void apiHostChanged();
    void canLogInChanged();

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

    std::optional<QString> getKeychainValue(const QString &prefix, const QString &key);
    void setKeychainValue(const QString &prefix, const QString &key, const QString &value);
    void wipeKeychainValue(const QString &prefix, const QString &key);

    /**
     * @brief Puts flatpak in the key name to prevent possible collisions.
     * @note Not necessary if using @f getKeychainValue or @f setKeychainValue, they call this internally.
     */
    QString transformKey(const QString &prefix, const QString &key);

    QNetworkAccessManager *m_netManager;
    QString m_region;
    QString m_language;
    QString m_apiHost;
};

}
