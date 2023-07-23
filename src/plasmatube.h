// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <optional>

namespace QInvidious
{
    class InvidiousApi;
}

class PlasmaTube : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY credentialsChanged)
    Q_PROPERTY(QString invidiousId READ invidiousId NOTIFY credentialsChanged)

public:
    explicit PlasmaTube(QObject *parent = nullptr);
    static PlasmaTube &instance();

    QInvidious::InvidiousApi *api() const;

    bool isLoggedIn() const;
    QString invidiousId() const;
    Q_SIGNAL void credentialsChanged();

    std::optional<bool> isSubscribedToChannel(const QString &jid) const;
    Q_SIGNAL void subscriptionsChanged();

    void fetchSubscriptions();
    Q_INVOKABLE void logOut();

    void loadCredentials();
    void saveCredentials() const;

    Q_SIGNAL void errorOccurred(const QString &errorText);
    Q_SIGNAL void loggedIn();
    Q_SIGNAL void loggedOut();

    bool isVideoWatched(const QString &videoId);
    Q_INVOKABLE void markVideoWatched(const QString &videoId);
    void markVideoUnwatched(const QString &videoId);

private:
    friend class SubscriptionController;

    void setSubscriptions(const QList<QString> &subscriptions);
    std::optional<QList<QString>> &subscriptions();

    void fetchHistory(qint32 page = 1);

    QInvidious::InvidiousApi *const m_api;
    std::optional<QList<QString>> m_subscriptions;
    QList<QString> m_watchedVideos;
};
