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
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>

InvidiousManager::InvidiousManager(QString invidiousInstance, QObject *parent)
    : QObject(parent), m_instance(invidiousInstance),
      m_netManager(new QNetworkAccessManager(this))
{
}

QNetworkReply* InvidiousManager::search(const QString &searchQuery, qint32 page)
{
    // TODO: Also support other parameters
    QUrl url(m_instance + "/api/v1/search");
    QUrlQuery query;
    query.addQueryItem("q", searchQuery);
    query.addQueryItem("page", QString::number(page));
    url.setQuery(query);

    QNetworkReply* reply = m_netManager->get(QNetworkRequest(url));

    // success
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull()) {
            emit searchFailed();
            return;
        }

        QList<VideoBasicInfo> results;
        foreach (const QJsonValue &val, doc.array()) {
            VideoBasicInfo video;
            video.parseFromJson(val.toObject());
            results.append(video);
        }

        emit searchResults(results);
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [this](QNetworkReply::NetworkError) {
        emit searchFailed();
    });

    return reply;
}
