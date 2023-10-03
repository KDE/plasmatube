// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// std
#include <variant>
// Qt
#include <QHash>
#include <QObject>
#include <QFuture>
#include <QNetworkReply>
#include <QNetworkCookie>
// QInvidious
#include "video.h"
#include "credentials.h"
#include "searchparameters.h"

class QNetworkAccessManager;

namespace QInvidious {

using Error = std::pair<QNetworkReply::NetworkError, QString>;
using Success = std::monostate;
using LogInResult = std::variant<Credentials, Error>;
using VideoResult = std::variant<Video, Error>;
using VideoListResult = std::variant<QList<VideoBasicInfo>, Error>;
using SubscriptionsResult = std::variant<QList<QString>, Error>;
using Result = std::variant<Success, Error>;
using HistoryResult = std::variant<QList<QString>, Error>;

enum TrendingTopic : quint8 {
    Main,
    Music,
    Gaming,
    Movies,
    News
};

class InvidiousApi : public QObject
{
    Q_OBJECT

public:
    explicit InvidiousApi(QNetworkAccessManager *netManager, QObject *parent = nullptr);

    QString region() const;
    void setRegion(const QString &region);

    QString language() const;
    void setLanguage(const QString &language);

    Credentials credentials() const;
    void setCredentials(const Credentials &credentials);
    void setCredentials(const QString &apiInstance);
    Q_SIGNAL void credentialsChanged();

    QString invidiousInstance() const;

    QFuture<LogInResult> logIn(QStringView username, QStringView password);
    QFuture<VideoResult> requestVideo(QStringView videoId);
    QFuture<VideoListResult> requestSearchResults(const SearchParameters &parameters);
    QFuture<VideoListResult> requestFeed(qint32 page = 1);
    QFuture<VideoListResult> requestTop();
    QFuture<VideoListResult> requestTrending(TrendingTopic = Main);
    QFuture<VideoListResult> requestChannel(QStringView query, qint32 page = 1);
    QFuture<SubscriptionsResult> requestSubscriptions();
    QFuture<Result> subscribeToChannel(QStringView channel);
    QFuture<Result> unsubscribeFromChannel(QStringView channel);
    QFuture<HistoryResult> requestHistory(qint32 page = 1);
    QFuture<Result> markWatched(const QString &videoId);
    QFuture<Result> markUnwatched(const QString &videoId);

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

    template<typename T>
    QFuture<T> reportResults(QNetworkReply *reply, std::function<T (QNetworkReply *)> processs)
    {
        auto interface = std::make_shared<QFutureInterface<T>>(QFutureInterfaceBase::Started);

        connect(reply, &QNetworkReply::finished, this, [=] () {
            interface->reportResult(processs(reply));
            interface->reportFinished();
            reply->deleteLater();
        });
        connect(reply, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError error) {
            interface->reportResult(std::pair(error, reply->errorString()));
            interface->reportFinished();
            reply->deleteLater();
        });

        return interface->future();
    }

    template<typename T>
    QFuture<T> get(QNetworkRequest &&request, std::function<T (QNetworkReply *)> processs)
    {
        return reportResults<T>(m_netManager->get(request), std::move(processs));
    }

    template<typename T>
    QFuture<T> post(QNetworkRequest &&request, const QByteArray &data, std::function<T (QNetworkReply *)> processs)
    {
        return reportResults<T>(m_netManager->post(request, data), std::move(processs));
    }

    template<typename T>
    QFuture<T> deleteResource(QNetworkRequest &&request, std::function<T (QNetworkReply *)> processs)
    {
        return reportResults<T>(m_netManager->deleteResource(request), std::move(processs));
    }

    QNetworkAccessManager *m_netManager;
    QString m_region;
    QString m_language;
    Credentials m_credentials;
};

using Api = InvidiousApi;

};
