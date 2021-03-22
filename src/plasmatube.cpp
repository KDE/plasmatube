// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "plasmatube.h"

#include <QSettings>
#include <QStringBuilder>
#include <QFutureWatcher>

#include "qinvidious/invidiousapi.h"

#include "constants.h"

PlasmaTube::PlasmaTube(QObject *parent)
    : QObject(parent),
      m_api(new QInvidious::Api(new QNetworkAccessManager(this), this))
{
    connect(m_api, &QInvidious::Api::credentialsChanged, this, &PlasmaTube::credentialsChanged);

    const auto locale = QLocale::system().name().toLower().split(u'_');
    m_api->setLanguage(locale.at(0));
    m_api->setRegion(locale.at(1));
    loadCredentials();

    if (isLoggedIn()) {
        fetchSubscriptions();
    }

    connect(this, &PlasmaTube::loggedOut, this, [=] {
        m_subscriptions.reset();
        emit subscriptionsChanged();
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
            emit errorOccurred(error->second);
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
        emit loggedOut();
    }
}

void PlasmaTube::loadCredentials()
{
    QSettings settings;
    QInvidious::Credentials credentials;
    credentials.setApiInstance(settings.value(SETTINGS_INSTANCE, DEFAULT_INSTANCE).toString());
    credentials.setUsername(settings.value(SETTINGS_USERNAME).toString());
    if (const auto cookies = QNetworkCookie::parseCookies(settings.value(SETTINGS_COOKIE).toByteArray());
            !cookies.isEmpty()) {
        credentials.setCookie(cookies.first());
    }
    m_api->setCredentials(credentials);
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
    emit subscriptionsChanged();
}

std::optional<QList<QString>> &PlasmaTube::subscriptions()
{
    return m_subscriptions;
}
