// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "accountmanager.h"

#include <QDebug>
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

#include "constants.h"

AccountManager::AccountManager(QObject *parent)
    : QObject(parent),
      m_netManager(new QNetworkAccessManager(this))
{
    QSettings settings;
    m_instance = settings.value(SETTINGS_INSTANCE, QStringLiteral(DEFAULT_INSTANCE)).toString();
    m_username = settings.value(SETTINGS_USERNAME).toString();

    QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(
        settings.value(SETTINGS_COOKIE).toByteArray()
    );
    if (!cookies.isEmpty())
        m_cookie = cookies.first();

    // fetch subscribed channels
    if (!m_username.isEmpty())
        fetchSubscriptions();
}

AccountManager::~AccountManager()
{
}

AccountManager &AccountManager::instance()
{
    static AccountManager instance;
    return instance;
}

void AccountManager::logIn(const QString &username, const QString &password)
{
    QUrl url(m_instance + INVIDIOUS_API_LOGIN);
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
        const auto cookies = reply->header(QNetworkRequest::SetCookieHeader).value<QList<QNetworkCookie>>();

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
    connect(reply, &QNetworkReply::errorOccurred,
            this, [=](QNetworkReply::NetworkError error) {
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
    connect(reply, &QNetworkReply::errorOccurred,
            this, [=](QNetworkReply::NetworkError) {
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
    connect(reply, &QNetworkReply::errorOccurred,
            this, [=](QNetworkReply::NetworkError error) {
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
    connect(reply, &QNetworkReply::errorOccurred,
            this, [=](QNetworkReply::NetworkError error) {
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
    return m_username + u'@' + QUrl(m_instance).host();
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
    if (!AccountManager::instance().username().isEmpty()) {
        request.setHeader(
            QNetworkRequest::CookieHeader,
            QVariant::fromValue(QList<QNetworkCookie>{AccountManager::instance().cookie()})
        );
    }
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
    settings.setValue(SETTINGS_INSTANCE, m_instance);
    settings.setValue(SETTINGS_USERNAME, m_username);
    settings.setValue(SETTINGS_COOKIE, m_cookie.toRawForm());
}
