// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDir>
#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "preferences.h"
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
    Q_PROPERTY(QInvidious::Preferences preferences READ preferences WRITE setPreferences NOTIFY preferencesChanged)

public:
    explicit VideoSource(const QString &key, QObject *parent = nullptr);

    enum class Type { Invidious, PeerTube, Piped };
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

    QInvidious::Preferences preferences();
    void setPreferences(const QInvidious::Preferences &preferences);

    Q_INVOKABLE bool isVideoWatched(const QString &videoId);
    Q_INVOKABLE void markVideoWatched(const QString &videoId);
    void markVideoUnwatched(const QString &videoId);

    std::optional<bool> isSubscribedToChannel(const QString &jid) const;

    Q_INVOKABLE void addToPlaylist(const QString &plid, const QString &videoId);

    bool hasFinishedLoading() const;

    QInvidious::AbstractApi *api() const;

    Q_INVOKABLE bool supportsPopularPage() const;
    Q_INVOKABLE bool supportsTrendingCategories() const;

Q_SIGNALS:
    void urlChanged();
    void typeChanged();
    void credentialsChanged();
    void usernameChanged();
    void preferencesChanged();
    void finishedLoading();
    void subscriptionsChanged();

private:
    friend class SubscriptionController;

    void createApi();
    void setApiCookie();
    QString cookieKey();
    void fetchPreferences();
    void fetchSubscriptions();
    void fetchHistory(qint32 page = 1);
    void setSubscriptions(const QList<QString> &subscriptions);
    std::optional<QList<QString>> &subscriptions();

    SourceConfig m_config;
    QString m_key;
    QInvidious::AbstractApi *m_api;
    QString m_cookie;
    QInvidious::Preferences m_preferences;
    std::optional<QList<QString>> m_subscriptions;
    QList<QString> m_watchedVideos;
    bool m_finishedLoading = false;
};
