// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstractapi.h"

namespace QInvidious
{

AbstractApi::AbstractApi(QNetworkAccessManager *netManager, QObject *parent)
    : QObject(parent)
    , m_netManager(netManager)
{
}

QString AbstractApi::region() const
{
    return m_region;
}

void AbstractApi::setRegion(const QString &region)
{
    m_region = region;
}

QString AbstractApi::language() const
{
    return m_language;
}

void AbstractApi::setLanguage(const QString &language)
{
    m_language = language;
}

Credentials AbstractApi::credentials() const
{
    return m_credentials;
}

void AbstractApi::setCredentials(const Credentials &credentials)
{
    m_credentials = credentials;
    Q_EMIT credentialsChanged();
}

QNetworkAccessManager *AbstractApi::net() const
{
    return m_netManager;
}

QUrl AbstractApi::apiUrl(const QString &path) const
{
    QUrl url = QUrl::fromUserInput(m_apiHost);
    url.setScheme(QStringLiteral("https"));
    url.setPath(path);

    return url;
}

void AbstractApi::setApiHost(const QString &host)
{
    m_apiHost = host;
}

QString AbstractApi::apiHost() const
{
    return m_apiHost;
}
}