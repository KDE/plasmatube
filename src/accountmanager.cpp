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

#include "accountmanager.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMetaEnum>
#include <QSettings>
#include <QUrl>
#include <QUrlQuery>

#define DEFAULT_INSTANCE "https://invidio.us"
#define INVIDIOUS_API_SUBSCRIPTIONS "/api/v1/auth/subscriptions"
#define INVIDIOUS_API_SUBSCRIPTIONS_ "/api/v1/auth/subscriptions/"

static AccountManager *s_instance;

AccountManager::AccountManager(QObject *parent)
    : QObject(parent),
      m_netManager(new QNetworkAccessManager(this))
{
    Q_ASSERT(!s_instance);
    s_instance = this;

    QSettings settings;
    m_instance = settings.value("auth/instance", QStringLiteral(DEFAULT_INSTANCE)).toString();
    m_username = settings.value("auth/username").toString();

    QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(
        settings.value("auth/cookie").toByteArray()
    );
    if (!cookies.isEmpty())
        m_cookie = cookies.first();

    // fetch subscribed channels
    if (!m_username.isEmpty())
        fetchSubscriptions();
}

AccountManager::~AccountManager()
{
    s_instance = nullptr;
}

AccountManager * AccountManager::instance()
{
    if (!s_instance)
        s_instance = new AccountManager();

    return s_instance;
}

void AccountManager::logIn(const QString &username, const QString &password)
{
    QUrl url(m_instance + "/login");
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("referer", QUrl::toPercentEncoding("/"));
    urlQuery.addQueryItem("type", "invidious");
    url.setQuery(urlQuery);

    QUrlQuery params;
    params.addQueryItem("email", QUrl::toPercentEncoding(username));
    params.addQueryItem("password", QUrl::toPercentEncoding(password));
    params.addQueryItem("action", "signin");

    auto request = QNetworkRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = m_netManager->post(request, params.toString().toUtf8());

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        const auto cookies = qvariant_cast<QList<QNetworkCookie>>(
            reply->header(QNetworkRequest::SetCookieHeader)
        );

        if (!cookies.isEmpty()) {
            m_username = username;
            emit usernameChanged();
            emit invidiousIdChanged();

            m_cookie = cookies.first();
            emit loggedIn();

            saveCredentials();

            fetchSubscriptions();
        } else {
            emit loggingInFailed(tr("Username or password is invalid."));
        }

        reply->deleteLater();
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError error) {
        emit loggingInFailed(
            QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(error)
        );

        reply->deleteLater();
    });
}

void AccountManager::logOut()
{
    m_username = QString();
    m_cookie = QNetworkCookie();
    emit usernameChanged();
    emit invidiousIdChanged();

    saveCredentials();

    emit loggedOut();

    // reset cache
    setSubscribedChannelIds({});
}

void AccountManager::fetchSubscriptions()
{
    QUrl url(m_instance + QStringLiteral(INVIDIOUS_API_SUBSCRIPTIONS));

    QUrlQuery query;
    query.addQueryItem("fields", "authorId");
    url.setQuery(query);

    QNetworkReply *reply = m_netManager->get(networkRequestWithCookie(url.toString()));

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isNull())
            return;

        QJsonArray array = doc.array();

        QVector<QString> subscriptions;
        subscriptions.reserve(array.size());

        for (const auto &item : qAsConst(array))
            subscriptions << item.toObject().value(QStringLiteral("authorId")).toString();

        setSubscribedChannelIds(subscriptions);

        reply->deleteLater();
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError) {
        reply->deleteLater();
    });
}

void AccountManager::subscribeToChannel(const QString &channelId)
{
    QUrl url(m_instance + QStringLiteral(INVIDIOUS_API_SUBSCRIPTIONS_) + channelId);

    QNetworkReply *reply = m_netManager->post(
        networkRequestWithCookie(url.toString()),
        QByteArray()
    );

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode >= 200 && statusCode < 300) {
            emit subscribedToChannel(channelId);

            if (!m_subscribedChannelIds.contains(channelId)) {
                m_subscribedChannelIds.append(channelId);
                emit subscribedChannelIdsChanged();
            }
        } else {
            emit subscribingFailed(channelId, QString::fromUtf8(reply->readAll()));
        }

        reply->deleteLater();
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError error) {
        emit subscribingFailed(
            channelId,
            QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(error)
        );

        reply->deleteLater();
    });
}

void AccountManager::unsubscribeFromChannel(const QString &channelId)
{
    QUrl url(m_instance + QStringLiteral(INVIDIOUS_API_SUBSCRIPTIONS_) + channelId);
    QNetworkReply *reply = m_netManager->deleteResource(networkRequestWithCookie(url.toString()));

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode >= 200 && statusCode < 300) {
            emit unsubscribedFromChannel(channelId);

            if (m_subscribedChannelIds.contains(channelId)) {
                m_subscribedChannelIds.removeAll(channelId);
                emit subscribedChannelIdsChanged();
            }
        } else {
            emit unsubscribingFailed(channelId, QString::fromUtf8(reply->readAll()));
        }

        reply->deleteLater();
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError error) {
        emit unsubscribingFailed(
            channelId,
            QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(error)
        );

        reply->deleteLater();
    });
}

QString AccountManager::username() const
{
    return m_username;
}

QString AccountManager::invidiousId() const
{
    return QStringLiteral("%1@%2").arg(m_username).arg(QUrl(m_instance).host());
}

QString AccountManager::invidiousInstance() const
{
    return m_instance;
}

QNetworkCookie AccountManager::cookie() const
{
    return m_cookie;
}

QVector<QString> AccountManager::subscribedChanneldIds() const
{
    return m_subscribedChannelIds;
}

QNetworkAccessManager *AccountManager::netManager()
{
    return m_netManager;
}

QNetworkRequest AccountManager::networkRequestWithCookie(const QString &url)
{
    QNetworkRequest request(url);
    if (!AccountManager::instance()->username().isEmpty())
        request.setHeader(
            QNetworkRequest::CookieHeader,
            QVariant::fromValue(QList<QNetworkCookie>() << AccountManager::instance()->cookie())
        );
    return request;
}

void AccountManager::setSubscribedChannelIds(const QVector<QString> &subs)
{
    m_subscribedChannelIds = subs;
    emit subscribedChannelIdsChanged();
}

void AccountManager::saveCredentials()
{
    QSettings settings;
    settings.setValue("auth/instance", m_instance);
    settings.setValue("auth/username", m_username);
    settings.setValue("auth/cookie", m_cookie.toRawForm());
}
