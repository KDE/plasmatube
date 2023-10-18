// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "plasmatube.h"

#include <KLocalizedString>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QSettings>
#include <QStringBuilder>

#include "qinvidious/invidiousapi.h"

#include "PlasmaTubeSettings.h"
#include "constants.h"

PlasmaTube::PlasmaTube(QObject *parent)
    : QObject(parent),
      m_api(new QInvidious::Api(new QNetworkAccessManager(this), this))
{
    connect(m_api, &QInvidious::Api::credentialsChanged, this, &PlasmaTube::credentialsChanged);

    const auto locale = QLocale::system().name().toLower().split(u'_');
    if (locale.size() == 2) {
        m_api->setLanguage(locale.at(0));
        // Regions seem to cause internal server errors. See https://invent.kde.org/plasma-mobile/plasmatube/-/issues/20 for details
        //m_api->setRegion(locale.at(1));
    }
    loadCredentials();

    if (isLoggedIn()) {
        fetchSubscriptions();
    }

    connect(this, &PlasmaTube::loggedOut, this, [=] {
        m_subscriptions.reset();
        Q_EMIT subscriptionsChanged();
    });
    connect(this, &PlasmaTube::loggedIn, this, [=] {
        fetchSubscriptions();
    });
}

PlasmaTube &PlasmaTube::instance()
{
    static PlasmaTube instance;
    return instance;
}

QInvidious::InvidiousApi *PlasmaTube::api() const
{
    return m_api;
}

bool PlasmaTube::isLoggedIn() const
{
    return !m_api->credentials().isAnonymous();
}

QString PlasmaTube::invidiousId() const
{
    const auto credentials = m_api->credentials();
    return credentials.username() % u'@' % QUrl(credentials.apiInstance()).host();
}

std::optional<bool> PlasmaTube::isSubscribedToChannel(const QString &jid) const
{
    if (m_subscriptions.has_value()) {
        return m_subscriptions->contains(jid);
    }
    return std::nullopt;
}

void PlasmaTube::fetchSubscriptions()
{
    auto *watcher = new QFutureWatcher<QInvidious::SubscriptionsResult>();
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher] {
        auto result = watcher->result();

        if (const auto subscriptions = std::get_if<QList<QString>>(&result)) {
            setSubscriptions(*subscriptions);
        } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
            qDebug() << "Fetching subscriptions:" << error->first << error->second;
            Q_EMIT errorOccurred(error->second);
        }

        watcher->deleteLater();
    });
    watcher->setFuture(m_api->requestSubscriptions());
}

void PlasmaTube::logOut()
{
    if (!m_api->credentials().isAnonymous()) {
        // set the credentials to only the invidious api instance
        m_api->setCredentials(m_api->credentials().apiInstance());
        saveCredentials();
        Q_EMIT loggedOut();
    }
}

void PlasmaTube::loadCredentials()
{
    const PlasmaTubeSettings plasmaTubeSettings;
    QSettings settings;
    QInvidious::Credentials credentials;
    credentials.setApiInstance(settings.value(SETTINGS_INSTANCE, plasmaTubeSettings.invidiousInstance()).toString());
    credentials.setUsername(settings.value(SETTINGS_USERNAME).toString());
    if (const auto cookies = QNetworkCookie::parseCookies(settings.value(SETTINGS_COOKIE).toByteArray());
            !cookies.isEmpty()) {
        credentials.setCookie(cookies.first());
    }
    m_api->setCredentials(credentials);
    fetchHistory();
    fetchPreferences();
}

void PlasmaTube::saveCredentials() const
{
    QSettings settings;
    auto credentials = m_api->credentials();
    settings.setValue(SETTINGS_INSTANCE, credentials.apiInstance());
    settings.setValue(SETTINGS_USERNAME, credentials.username());
    if (credentials.cookie().has_value()) {
        settings.setValue(SETTINGS_COOKIE, credentials.cookie()->toRawForm());
    } else {
        settings.remove(SETTINGS_COOKIE);
    }
}

void PlasmaTube::setSubscriptions(const QList<QString> &subscriptions)
{
    m_subscriptions = subscriptions;
    Q_EMIT subscriptionsChanged();
}

std::optional<QList<QString>> &PlasmaTube::subscriptions()
{
    return m_subscriptions;
}

bool PlasmaTube::isVideoWatched(const QString &videoId)
{
    return m_watchedVideos.contains(videoId);
}

void PlasmaTube::markVideoWatched(const QString &videoId)
{
    if (!m_watchedVideos.contains(videoId) && isLoggedIn()) {
        m_watchedVideos.push_back(videoId);
        m_api->markWatched(videoId);
    }
}

void PlasmaTube::markVideoUnwatched(const QString &videoId)
{
    if (m_watchedVideos.contains(videoId) && isLoggedIn()) {
        m_watchedVideos.removeAll(videoId);
        m_api->markUnwatched(videoId);
    }
}

void PlasmaTube::fetchHistory(qint32 page)
{
    if (!isLoggedIn()) {
        return;
    }

    if (page == 1) {
        m_watchedVideos.clear();
    }

    auto *watcher = new QFutureWatcher<QInvidious::HistoryResult>();
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, page] {
        auto result = watcher->result();

        if (const auto history = std::get_if<QList<QString>>(&result)) {
            m_watchedVideos.append(*history);

            fetchHistory(page + 1);
        }

        watcher->deleteLater();
    });
    watcher->setFuture(m_api->requestHistory(page));
}

void PlasmaTube::setInhibitSleep(const bool inhibit)
{
#ifdef HAS_DBUS
    if (inhibit) {
        QDBusMessage message = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                              QStringLiteral("/ScreenSaver"),
                                                              QStringLiteral("org.freedesktop.ScreenSaver"),
                                                              QStringLiteral("Inhibit"));
        message << QGuiApplication::desktopFileName();
        message << i18n("Playing video");

        QDBusReply<uint> reply = QDBusConnection::sessionBus().call(message);
        if (reply.isValid()) {
            screenSaverDbusCookie = reply.value();
        }
    } else {
        if (screenSaverDbusCookie != 0) {
            QDBusMessage message = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                                  QStringLiteral("/ScreenSaver"),
                                                                  QStringLiteral("org.freedesktop.ScreenSaver"),
                                                                  QStringLiteral("UnInhibit"));
            message << static_cast<uint>(screenSaverDbusCookie);
            screenSaverDbusCookie = 0;
            QDBusConnection::sessionBus().send(message);
        }
    }
#endif
}

QInvidious::Preferences PlasmaTube::preferences()
{
    return m_preferences;
}

void PlasmaTube::setPreferences(const QInvidious::Preferences &preferences)
{
    if (!isLoggedIn()) {
        return;
    }

    m_api->setPreferences(preferences);
    m_preferences = preferences;
    Q_EMIT preferencesChanged();
}

void PlasmaTube::fetchPreferences()
{
    if (!isLoggedIn()) {
        return;
    }

    auto *watcher = new QFutureWatcher<QInvidious::PreferencesResult>();
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher] {
        auto result = watcher->result();

        if (const auto prefs = std::get_if<QInvidious::Preferences>(&result)) {
            m_preferences = *prefs;
            Q_EMIT preferencesChanged();
        }

        watcher->deleteLater();
    });
    watcher->setFuture(m_api->requestPreferences());
}

void PlasmaTube::addToPlaylist(const QString &plid, const QString &videoId)
{
    PlasmaTube::instance().api()->addVideoToPlaylist(plid, videoId);
}
