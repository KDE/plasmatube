// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <optional>

#include "preferences.h"

namespace QInvidious
{
    class InvidiousApi;
}

class PlasmaTube : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY credentialsChanged)
    Q_PROPERTY(QString invidiousId READ invidiousId NOTIFY credentialsChanged)
    Q_PROPERTY(QInvidious::Preferences preferences READ preferences WRITE setPreferences NOTIFY preferencesChanged)

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

    Q_INVOKABLE bool isVideoWatched(const QString &videoId);
    Q_INVOKABLE void markVideoWatched(const QString &videoId);
    void markVideoUnwatched(const QString &videoId);

    Q_INVOKABLE void setInhibitSleep(bool inhibit);

    QInvidious::Preferences preferences();
    void setPreferences(const QInvidious::Preferences &preferences);

    Q_INVOKABLE void addToPlaylist(const QString &plid, const QString &videoId);

Q_SIGNALS:
    void openVideo(const QString &id);
    void preferencesChanged();

private:
    friend class SubscriptionController;

    void setSubscriptions(const QList<QString> &subscriptions);
    std::optional<QList<QString>> &subscriptions();

    void fetchHistory(qint32 page = 1);

    void fetchPreferences();

    QInvidious::InvidiousApi *const m_api;
    std::optional<QList<QString>> m_subscriptions;
    QList<QString> m_watchedVideos;
    unsigned int screenSaverDbusCookie = 0;
    QInvidious::Preferences m_preferences;
};
