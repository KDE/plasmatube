// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "invidiousmanager.h"
// Qt
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMetaEnum>
#include <QJsonDocument>
#include <QJsonArray>

#include "accountmanager.h"
#include "constants.h"

using namespace QInvidious;

InvidiousManager::InvidiousManager(QObject *parent)
    : QObject(parent),
      m_region(QLocale::system().name().split("_").first())
{
}

QString InvidiousManager::region() const
{
    return m_region;
}

void InvidiousManager::setRegion(const QString &region)
{
    m_region = region;
}

QNetworkReply *InvidiousManager::requestVideo(const QString &videoId)
{
    QUrl url(invidiousInstance() + INVIDIOUS_API_VIDEOS_ + videoId);
    QUrlQuery query;
    query.addQueryItem("region", m_region);
    url.setQuery(query);

    QNetworkReply* reply = netManager()->get(QNetworkRequest(url));

    // success
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            emit videoRequestFailed();
            return;
        }
        emit videoReceived(doc.object());
    });

    // failure
    connect(reply, &QNetworkReply::errorOccurred,
            this, [this](QNetworkReply::NetworkError) {
        emit videoRequestFailed();
    });
    return reply;
}

QNetworkReply* InvidiousManager::videoQuery(VideoListType queryType,
                                            const QString &queryValue,
                                            qint32 page)
{
    QString urlString = AccountManager::instance().invidiousInstance();

    QUrlQuery query;
    query.addQueryItem("region", m_region);

    switch (queryType) {
    case Search:
        urlString.append(INVIDIOUS_API_SEARCH);

        query.addQueryItem("q", QUrl::toPercentEncoding(queryValue));
        query.addQueryItem("page", QString::number(page));
        break;
    case Trending:
        urlString.append(INVIDIOUS_API_TRENDING);

        if (!queryValue.isEmpty())
            query.addQueryItem("type", queryValue);
        break;
    case Top:
        urlString.append(INVIDIOUS_API_TOP);
        break;
    case Feed:
        urlString.append(INVIDIOUS_API_FEED);
        break;
    }

    QUrl url(urlString);
    url.setQuery(query);

    QNetworkRequest request(url.toString());
    if (!AccountManager::instance().username().isEmpty()) {
        request.setHeader(
            QNetworkRequest::CookieHeader,
            QVariant::fromValue(QList<QNetworkCookie> { AccountManager::instance().cookie() })
        );
    }

    QNetworkReply *reply = netManager()->get(request);

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            emit videoQueryFailed(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString());
            return;
        }

        QList<VideoBasicInfo> results;

        if (queryType == Feed) {
            const QJsonObject obj = doc.object();

            const auto notifications = obj.value("notifications").toArray();
            for (const QJsonValue &val : notifications) {
                auto video = VideoBasicInfo::fromJson(val);
                video.setIsNotification(true);
                results << video;
            }

            results << VideoBasicInfo::fromJson(obj.value("videos").toArray());
        } else {
            results = VideoBasicInfo::fromJson(doc.array());
        }

        emit videoQueryResults(results);
    });

    // failure
    connect(reply, &QNetworkReply::errorOccurred,
            this, [=] (QNetworkReply::NetworkError error) {
        emit videoQueryFailed(QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(error));
    });

    return reply;
}

QString InvidiousManager::invidiousInstance()
{
    return AccountManager::instance().invidiousInstance();
}

QNetworkAccessManager *InvidiousManager::netManager()
{
    return AccountManager::instance().netManager();
}
