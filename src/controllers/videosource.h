// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "paginator.h"
#include "preferences.h"
#include "sourceconfig.h"
#include "subscriptionlistmodel.h"

#include <QDir>
#include <QObject>
#include <QtQml>

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
    Q_PROPERTY(bool canLogIn READ canLogIn NOTIFY canLogInChanged)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
    Q_PROPERTY(QInvidious::Preferences preferences READ preferences WRITE setPreferences NOTIFY preferencesChanged)
    Q_PROPERTY(bool showRelatedVideos READ showRelatedVideos CONSTANT)
    Q_PROPERTY(QString cookiesFromBrowser READ cookiesFromBrowser WRITE setCookiesFromBrowser NOTIFY cookiesFromBrowserChanged)
    Q_PROPERTY(QString cookiesBrowserProfile READ cookiesBrowserProfile WRITE setCookiesBrowserProfile NOTIFY cookiesBrowserProfileChanged)

public:
    explicit VideoSource(const QString &key, QObject *parent = nullptr);

    enum class Type { Invidious, PeerTube, Piped, YouTube };
    Q_ENUM(Type)

    [[nodiscard]] QString uuid() const;

    [[nodiscard]] QString url() const;
    void setUrl(const QString &url);

    [[nodiscard]] Type type() const;
    void setType(Type type);

    bool loggedIn() const;
    bool canLogIn() const;
    Q_INVOKABLE void prepareLogIn();
    Q_INVOKABLE void logOut();

    void setUsername(const QString &username);
    QString username() const;

    QString cookiesFromBrowser() const;
    void setCookiesFromBrowser(const QString &value);

    QString cookiesBrowserProfile() const;
    void setCookiesBrowserProfile(const QString &value);

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

    std::optional<QList<QString>> &subscriptions();

    // TODO: support PeerTube JSON (see https://github.com/Chocobozzz/PeerTube/pull/6215)
    /**
     * Imports subscriptions from @p filePath.
     */
    Q_INVOKABLE void importSubscriptions(const QString &filePath);

    // TODO: we should really not attach this to the model, if we want to move the import/export button somewhere else later.
    /**
     * Exports subscriptions to @p filePath. Requires @p model to reuse the channel data.
     */
    Q_INVOKABLE void exportSubscriptions(SubscriptionListModel *model, const QString &filePath);

    bool showRelatedVideos() const;

Q_SIGNALS:
    void urlChanged();
    void typeChanged();
    void credentialsChanged();
    void usernameChanged();
    void preferencesChanged();
    void finishedLoading();
    void subscriptionsChanged();
    void canLogInChanged();
    void importExportCompleted(int numSubscriptions);
    void importExportError(const QString &reason);
    void cookiesFromBrowserChanged();
    void cookiesBrowserProfileChanged();

private:
    friend class SubscriptionController;

    void createApi();
    void fetchPreferences();
    void fetchSubscriptions();
    void fetchHistory();
    void setSubscriptions(const QList<QString> &subscriptions);

    SourceConfig m_config;
    QString m_key;
    QInvidious::AbstractApi *m_api = nullptr;
    QInvidious::Preferences m_preferences;
    std::optional<QList<QString>> m_subscriptions;
    QList<QString> m_watchedVideos;
    bool m_finishedLoading = false;
    QInvidious::Paginator m_historyPagination;
};
