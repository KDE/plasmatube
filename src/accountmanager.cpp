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
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMetaEnum>
#include <QSettings>
#include <QUrl>
#include <QUrlQuery>

#define DEFAULT_INSTANCE "https://invidio.us"

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

void AccountManager::saveCredentials()
{
    QSettings settings;
    settings.setValue("auth/instance", m_instance);
    settings.setValue("auth/username", m_username);
    settings.setValue("auth/cookie", m_cookie.toRawForm());
}
