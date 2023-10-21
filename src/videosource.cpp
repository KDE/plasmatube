// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videosource.h"

#include <QFutureWatcher>
#include <qt6keychain/keychain.h>

#include "invidious/invidiousapi.h"
#include "peertube/peertubeapi.h"

VideoSource::VideoSource(const QString &key, QObject *parent)
    : QObject(parent)
    , m_config(key)
    , m_key(key)
{
    createApi();

    auto loop = new QEventLoop();

    auto job = new QKeychain::ReadPasswordJob(QStringLiteral("PlasmaTube"));
    job->setKey(cookieKey());
    job->start();

    QString value;

    QObject::connect(job, &QKeychain::ReadPasswordJob::finished, [loop, job, &value](QKeychain::Job *j) {
        value = job->textData();
        loop->quit();
    });

    loop->exec();

    if (!value.isEmpty()) {
        m_cookie = value;
        setApiCookie();
    }

    fetchPreferences();
}

QString VideoSource::uuid() const
{
    return m_key;
}

QString VideoSource::url() const
{
    return m_config.url();
}

void VideoSource::setUrl(const QString &url)
{
    if (m_config.url() != url) {
        m_api->setApiHost(url);
        m_config.setUrl(url);
        m_config.save();
        Q_EMIT urlChanged();
    }
}

VideoSource::Type VideoSource::type() const
{
    return static_cast<VideoSource::Type>(m_config.type());
}

void VideoSource::setType(const VideoSource::Type value)
{
    if (static_cast<VideoSource::Type>(m_config.type()) != value) {
        m_config.setType(static_cast<int>(value));
        m_config.save();
        Q_EMIT typeChanged();

        createApi();
    }
}

bool VideoSource::loggedIn() const
{
    return !m_cookie.isEmpty() && !username().isEmpty();
}

void VideoSource::logOut()
{
    auto cookieDeleteJob = new QKeychain::DeletePasswordJob{QStringLiteral("PlasmaTube"), this};
    cookieDeleteJob->setKey(cookieKey());
    cookieDeleteJob->start();

    setUsername(QStringLiteral(""));
    m_cookie.clear();

    Q_EMIT credentialsChanged();
}

QString VideoSource::username() const
{
    return m_config.username();
}

void VideoSource::setUsername(const QString &username)
{
    if (m_config.username() != username) {
        m_config.setUsername(username);
        m_config.save();
        Q_EMIT usernameChanged();
    }
}

void VideoSource::setCookie(const QString &cookie)
{
    m_cookie = cookie;

    auto cookieJob = new QKeychain::WritePasswordJob{QStringLiteral("PlasmaTube"), this};
    cookieJob->setKey(cookieKey());
    cookieJob->setTextData(cookie);
    cookieJob->start();

    setApiCookie();

    Q_EMIT credentialsChanged();
}

QString VideoSource::cookie() const
{
    return m_cookie;
}

QInvidious::Preferences VideoSource::preferences()
{
    return m_preferences;
}

void VideoSource::setPreferences(const QInvidious::Preferences &preferences)
{
    if (!loggedIn()) {
        return;
    }

    m_api->setPreferences(preferences);
    m_preferences = preferences;
    Q_EMIT preferencesChanged();
}

void VideoSource::fetchPreferences()
{
    if (!loggedIn()) {
        m_finishedLoading = true;
        Q_EMIT finishedLoading();
        return;
    }

    auto *watcher = new QFutureWatcher<QInvidious::PreferencesResult>();
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher] {
        auto result = watcher->result();

        if (const auto prefs = std::get_if<QInvidious::Preferences>(&result)) {
            m_preferences = *prefs;
            Q_EMIT preferencesChanged();
        }

        m_finishedLoading = true;
        Q_EMIT finishedLoading();

        watcher->deleteLater();
    });
    watcher->setFuture(m_api->requestPreferences());
}

bool VideoSource::hasFinishedLoading() const
{
    return m_finishedLoading;
}

QInvidious::AbstractApi *VideoSource::api() const
{
    return m_api;
}

void VideoSource::createApi()
{
    switch (type()) {
    case Type::Invidious:
        m_api = new QInvidious::InvidiousApi(new QNetworkAccessManager(this), this);
        break;
    case Type::PeerTube:
        m_api = new QInvidious::PeerTubeApi(new QNetworkAccessManager(this), this);
        break;
    }
    connect(m_api, &QInvidious::AbstractApi::credentialsChanged, this, &VideoSource::credentialsChanged);
    m_api->setApiHost(m_config.url());
}

void VideoSource::setApiCookie()
{
    m_api->setCredentials(QInvidious::Credentials(username(), m_cookie));
}

QString VideoSource::cookieKey()
{
#ifdef PLASMATUBE_FLATPAK
    return QStringLiteral("%1-flatpak-cookie").arg(m_key);
#else
    return QStringLiteral("%1-cookie").arg(m_key);
#endif
}
