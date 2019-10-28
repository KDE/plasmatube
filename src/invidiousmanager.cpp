/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "invidiousmanager.h"
// Qt
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>

#include "accountmanager.h"

InvidiousManager::InvidiousManager(QString invidiousInstance, QObject *parent)
    : QObject(parent),
      m_instance(invidiousInstance),
      m_region(QLocale::system().name().split("_").first()),
      m_netManager(new QNetworkAccessManager(this))
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
    QUrl url(m_instance + "/api/v1/videos/" + videoId);
    QUrlQuery query;
    query.addQueryItem("region", m_region);
    url.setQuery(query);

    QNetworkReply* reply = m_netManager->get(QNetworkRequest(url));

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
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [this](QNetworkReply::NetworkError) {
        emit videoRequestFailed();
    });
    return reply;
}

QNetworkReply* InvidiousManager::videoQuery(VideoListType queryType,
                                            const QString &queryValue,
                                            qint32 page)
{
    QString urlString = AccountManager::instance()->invidiousInstance();

    QUrlQuery query;
    query.addQueryItem("region", m_region);

    switch (queryType) {
    case Search:
        urlString.append("/api/v1/search");

        query.addQueryItem("q", QUrl::toPercentEncoding(queryValue));
        query.addQueryItem("page", QString::number(page));
        break;
    case Trending:
        urlString.append("/api/v1/trending");

        if (!queryValue.isEmpty())
            query.addQueryItem("type", queryValue);
        break;
    case Feed:
        urlString.append("/api/v1/auth/feed");
        break;
    }

    QUrl url(urlString);
    url.setQuery(query);

    QNetworkRequest request(url.toString());
    if (!AccountManager::instance()->username().isEmpty())
        request.setHeader(
            QNetworkRequest::CookieHeader,
            QVariant::fromValue(QList<QNetworkCookie>() << AccountManager::instance()->cookie())
        );

    QNetworkReply *reply = m_netManager->get(request);

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            emit videoQueryFailed();
            return;
        }

        QList<VideoBasicInfo> results;

        if (queryType == Feed) {
            const QJsonObject obj = doc.object();

            const QJsonArray notifications = obj.value("notifications").toArray();
            for (const QJsonValue &val : notifications) {
                VideoBasicInfo video;
                video.parseFromJson(val.toObject());
                video.setIsNotification(true);
                results.append(video);
            }

            const QJsonArray videos = obj.value("videos").toArray();
            for (const QJsonValue &val : notifications) {
                VideoBasicInfo video;
                video.parseFromJson(val.toObject());
                results.append(video);
            }
        } else {
            for (const QJsonValue &val : doc.array()) {
                VideoBasicInfo video;
                video.parseFromJson(val.toObject());
                results.append(video);
            }
        }

        emit videoQueryResults(results);
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError error) {
        emit videoQueryFailed();
    });

    return reply;
}
