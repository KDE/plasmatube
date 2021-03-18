// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QObject>
#include <QNetworkCookie>
#include <QVector>

class QNetworkAccessManager;
class QNetworkRequest;

class AccountManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
    Q_PROPERTY(QString invidiousId READ invidiousId NOTIFY invidiousIdChanged)
    Q_PROPERTY(QVector<QString> subscribedChanneldIds READ subscribedChanneldIds NOTIFY subscribedChannelIdsChanged)

public:
    AccountManager(QObject *parent = nullptr);
    ~AccountManager();

    static AccountManager &instance();

    Q_INVOKABLE void logIn(const QString &username, const QString &password);
    Q_INVOKABLE void logOut();

    Q_INVOKABLE void fetchSubscriptions();
    Q_INVOKABLE void subscribeToChannel(const QString &channelId);
    Q_INVOKABLE void unsubscribeFromChannel(const QString &channelId);

    QString username() const;
    QString invidiousId() const;
    QString invidiousInstance() const;
    QNetworkCookie cookie() const;

    QVector<QString> subscribedChanneldIds() const;

    QNetworkAccessManager *netManager();
    static QNetworkRequest networkRequestWithCookie(const QString &url);

signals:
    void loggedIn();
    void loggingInFailed(const QString &errorText);

    void loggedOut();

    void subscribedToChannel(const QString &channelId);
    void subscribingFailed(const QString &channelId, const QString &errorText);
    void unsubscribedFromChannel(const QString &channelId);
    void unsubscribingFailed(const QString &channelId, const QString &errorText);

    void usernameChanged();
    void invidiousIdChanged();

    void subscribedChannelIdsChanged();

private:
    void setSubscribedChannelIds(const QVector<QString> &);

    void saveCredentials();
    
    QNetworkAccessManager *m_netManager;
    QString m_username;
    QString m_instance;
    QNetworkCookie m_cookie;

    QVector<QString> m_subscribedChannelIds;
};

#endif // ACCOUNTMANAGER_H
