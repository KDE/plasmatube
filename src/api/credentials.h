// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QNetworkCookie>

#include <optional>

namespace QInvidious
{

class Credentials
{
public:
    Credentials() = default;
    Credentials(QString username, QString cookie);

    QString username() const;
    void setUsername(const QString &username);
    void setUsername(QStringView username);

    std::optional<QNetworkCookie> cookie() const;
    void setCookie(const std::optional<QNetworkCookie> &cookie);

    bool isAnonymous() const;

private:
    QString m_username;
    std::optional<QNetworkCookie> m_cookie;
};

}
