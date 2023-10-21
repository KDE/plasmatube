// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDir>
#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "sourceconfig.h"

namespace QInvidious
{
class AbstractApi;
}

class VideoSource : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Use from SourceManager")

    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY credentialsChanged)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)

public:
    explicit VideoSource(const QString &key, QObject *parent = nullptr);

    enum class Type { Invidious, PeerTube };
    Q_ENUM(Type)

    [[nodiscard]] QString uuid() const;

    [[nodiscard]] QString url() const;
    void setUrl(const QString &url);

    [[nodiscard]] Type type() const;
    void setType(Type type);

    bool loggedIn() const;
    Q_INVOKABLE void logOut();

    void setUsername(const QString &username);
    QString username() const;

    void setCookie(const QString &cookie);
    QString cookie() const;

    QInvidious::AbstractApi *api() const;

Q_SIGNALS:
    void urlChanged();
    void typeChanged();
    void credentialsChanged();
    void usernameChanged();

private:
    void createApi();
    void setApiCookie();
    QString cookieKey();

    SourceConfig m_config;
    QString m_key;
    QInvidious::AbstractApi *m_api;
    QString m_cookie;
};
