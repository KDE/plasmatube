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

#include "qinvidious/invidious/invidiousapi.h"

PlasmaTube::PlasmaTube(QObject *parent)
    : QObject(parent)
    , m_controller(new VideoController(this))
    , m_sourceManager(new SourceManager(this))
{
    m_sourceManager->load();
}

PlasmaTube &PlasmaTube::instance()
{
    static PlasmaTube instance;
    return instance;
}

VideoController *PlasmaTube::videoController() const
{
    return m_controller;
}

SourceManager *PlasmaTube::sourceManager() const
{
    return m_sourceManager;
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
    watcher->setFuture(PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestSubscriptions());
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
    auto currentSource = m_sourceManager->selectedSource();
    Q_ASSERT(currentSource != nullptr);

    if (!m_watchedVideos.contains(videoId) && currentSource->loggedIn()) {
        m_watchedVideos.push_back(videoId);
        currentSource->api()->markWatched(videoId);
    }
}

void PlasmaTube::markVideoUnwatched(const QString &videoId)
{
    auto currentSource = m_sourceManager->selectedSource();
    Q_ASSERT(currentSource != nullptr);

    if (m_watchedVideos.contains(videoId) && currentSource->loggedIn()) {
        m_watchedVideos.removeAll(videoId);
        currentSource->api()->markUnwatched(videoId);
    }
}

void PlasmaTube::fetchHistory(qint32 page)
{
    auto currentSource = m_sourceManager->selectedSource();
    Q_ASSERT(currentSource != nullptr);

    if (!currentSource->loggedIn()) {
        return;
    }

    if (page == 1) {
        m_watchedVideos.clear();
    }

    auto *watcher = new QFutureWatcher<QInvidious::HistoryResult>();
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, page] {
        auto result = watcher->result();

        if (const auto history = std::get_if<QList<QString>>(&result)) {
            if (!history->isEmpty()) {
                m_watchedVideos.append(*history);

                fetchHistory(page + 1);
            }
        }

        watcher->deleteLater();
    });
    watcher->setFuture(currentSource->api()->requestHistory(page));
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
    auto currentSource = m_sourceManager->selectedSource();
    Q_ASSERT(currentSource != nullptr);

    if (!currentSource->loggedIn()) {
        return;
    }

    currentSource->api()->setPreferences(preferences);
    m_preferences = preferences;
    Q_EMIT preferencesChanged();
}

void PlasmaTube::fetchPreferences()
{
    auto currentSource = m_sourceManager->selectedSource();
    Q_ASSERT(currentSource != nullptr);

    if (!currentSource->loggedIn()) {
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
    watcher->setFuture(currentSource->api()->requestPreferences());
}

void PlasmaTube::addToPlaylist(const QString &plid, const QString &videoId)
{
    PlasmaTube::instance().sourceManager()->selectedSource()->api()->addVideoToPlaylist(plid, videoId);
}
