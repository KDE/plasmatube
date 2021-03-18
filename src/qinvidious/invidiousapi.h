// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// std
#include <variant>
// Qt
#include <QObject>
#include <QFuture>
#include <QNetworkReply>
#include <QNetworkCookie>
// QInvidious
#include "video.h"
#include "credentials.h"

class QNetworkAccessManager;

namespace QInvidious {

using Error = std::pair<QNetworkReply::NetworkError, QString>;
using Success = std::monostate;
using LogInResult = std::variant<Credentials, Error>;
using VideoResult = std::variant<Video, Error>;
using VideoListResult = std::variant<QList<VideoBasicInfo>, Error>;
using SubscriptionsResult = std::variant<QList<QString>, Error>;
using Result = std::variant<Success, Error>;

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

    Credentials credentials() const;
    void setCredentials(const Credentials &credentials);
    void setCredentials(const QString &apiInstance);

    QString invidiousInstance() const;

    QFuture<LogInResult> logIn(QStringView username, QStringView password);
    QFuture<VideoResult> requestVideo(QStringView videoId);
    QFuture<VideoListResult> requestSearchResults(QStringView query, qint32 page = 0);
    QFuture<VideoListResult> requestFeed();
    QFuture<VideoListResult> requestTop();
    QFuture<VideoListResult> requestTrending(TrendingTopic = Main);
    QFuture<SubscriptionsResult> requestSubscriptions();
    QFuture<Result> subscribeToChannel(QStringView channel);
    QFuture<Result> unsubscribeFromChannel(QStringView channel);

private:
    enum VideoListType {
        Search,
        Trending,
        Top,
        Feed
    };

    static Error invalidJsonError();
    static Result checkIsReplyOk(QNetworkReply *reply);

    QFuture<VideoListResult> requestVideoList(VideoListType queryType, QStringView queryValue = {}, qint32 page = 0);
    QNetworkRequest authenticatedNetworkRequest(QUrl &&url);
    QUrlQuery genericUrlQuery() const;
    QUrl logInUrl() const;
    QUrl videoUrl(QStringView videoId) const;
    QUrl videoListUrl(VideoListType queryType, QStringView queryValue, qint32 page) const;
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
    Credentials m_credentials;
};

using Api = InvidiousApi;

};
