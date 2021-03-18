// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "credentials.h"

using namespace QInvidious;

QString Credentials::apiInstance() const
{
    return m_apiInstance;
}

void Credentials::setApiInstance(const QString &instance)
{
    m_apiInstance = instance;
}

void Credentials::setApiInstance(QStringView instance)
{
    m_apiInstance = instance.toString();
}

QString Credentials::username() const
{
    return m_username;
}

void Credentials::setUsername(const QString &username)
{
    m_username = username;
}

void Credentials::setUsername(QStringView username)
{
    m_username = username.toString();
}

std::optional<QNetworkCookie> Credentials::cookie() const
{
    return m_cookie;
}

void Credentials::setCookie(const std::optional<QNetworkCookie> &cookie)
{
    m_cookie = cookie;
}

bool Credentials::isAnonymous() const
{
    return !m_cookie.has_value() || m_username.isEmpty();
}

