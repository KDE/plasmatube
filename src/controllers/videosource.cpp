// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videosource.h"
#include "opmlparser.h"

#include <QFutureWatcher>

#include <invidious/invidiousapi.h>
#include <peertube/peertubeapi.h>
#include <piped/pipedapi.h>
#include <qt6keychain/keychain.h>

VideoSource::VideoSource(const QString &key, QObject *parent)
    : QObject(parent)
    , m_config(key)
    , m_key(key)
    , m_historyPagination(this)
{
    createApi();
    fetchPreferences();
    fetchHistory();
    fetchSubscriptions();
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
    return m_api->isLoggedIn();
}

bool VideoSource::canLogIn() const
{
    return m_api->canLogIn();
}

void VideoSource::prepareLogIn()
{
    m_api->prepareLogIn();
}

void VideoSource::logOut()
{
    m_api->wipeCredentials(m_key);
    setUsername(QStringLiteral(""));

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

    // Check if the API supports preferences
    if (auto future = m_api->requestPreferences(); !future.isCanceled()) {
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
        watcher->setFuture(future);
    } else {
        m_finishedLoading = true;
        Q_EMIT finishedLoading();
    }
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
    case Type::Piped:
        m_api = new QInvidious::PipedApi(new QNetworkAccessManager(this), this);
        break;
    }
    connect(m_api, &QInvidious::AbstractApi::credentialsChanged, this, &VideoSource::credentialsChanged);
    connect(m_api, &QInvidious::AbstractApi::canLogInChanged, this, &VideoSource::canLogInChanged);
    m_api->setApiHost(m_config.url());
    m_api->setApiProtocol(m_config.protocol());
    m_api->loadCredentials(m_key);
}

std::optional<bool> VideoSource::isSubscribedToChannel(const QString &jid) const
{
    if (m_subscriptions.has_value()) {
        return m_subscriptions->contains(jid);
    }
    return std::nullopt;
}

void VideoSource::fetchSubscriptions()
{
    if (!loggedIn()) {
        return;
    }

    auto *watcher = new QFutureWatcher<QInvidious::SubscriptionsResult>();
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher] {
        auto result = watcher->result();

        if (const auto subscriptions = std::get_if<QList<QString>>(&result)) {
            setSubscriptions(*subscriptions);
        } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
            qDebug() << "Fetching subscriptions:" << error->first << error->second;
            // Q_EMIT errorOccurred(error->second);
        }

        watcher->deleteLater();
    });
    watcher->setFuture(m_api->requestSubscriptions());
}

void VideoSource::setSubscriptions(const QList<QString> &subscriptions)
{
    m_subscriptions = subscriptions;
    Q_EMIT subscriptionsChanged();
}

std::optional<QList<QString>> &VideoSource::subscriptions()
{
    return m_subscriptions;
}

bool VideoSource::isVideoWatched(const QString &videoId)
{
    return m_watchedVideos.contains(videoId);
}

void VideoSource::markVideoWatched(const QString &videoId)
{
    if (!m_watchedVideos.contains(videoId) && loggedIn()) {
        m_watchedVideos.push_back(videoId);
        m_api->markWatched(videoId);
    }
}

void VideoSource::markVideoUnwatched(const QString &videoId)
{
    if (m_watchedVideos.contains(videoId) && loggedIn()) {
        m_watchedVideos.removeAll(videoId);
        m_api->markUnwatched(videoId);
    }
}

void VideoSource::fetchHistory()
{
    if (!loggedIn()) {
        return;
    }

    if (auto future = m_api->requestHistory(&m_historyPagination); !future.isCanceled()) {
        auto *watcher = new QFutureWatcher<QInvidious::HistoryResult>();
        connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher] {
            auto result = watcher->result();

            if (const auto history = std::get_if<QList<QString>>(&result)) {
                if (!history->isEmpty()) {
                    m_watchedVideos.append(*history);

                    m_historyPagination.next();
                    fetchHistory();
                }
            }

            watcher->deleteLater();
        });
        watcher->setFuture(future);
    }
}

void VideoSource::addToPlaylist(const QString &plid, const QString &videoId)
{
    m_api->addVideoToPlaylist(plid, videoId);
}

bool VideoSource::supportsPopularPage() const
{
    return m_api->supportsFeature(QInvidious::AbstractApi::PopularPage);
}

bool VideoSource::supportsTrendingCategories() const
{
    return m_api->supportsFeature(QInvidious::AbstractApi::TrendingCategories);
}

void VideoSource::importSubscriptions(const QString &filePath)
{
    const auto result = OPMLParser::parseOPMLSubscriptions(filePath);
    if (const auto channelIds = std::get_if<QList<QString>>(&result)) {
        int newSubscriptions = 0;
        for (const auto &id : *channelIds) {
            if (!m_subscriptions->contains(id)) {
                m_api->subscribeToChannel(id);
                m_subscriptions->push_back(id);
                newSubscriptions++;
            }
        }

        Q_EMIT importExportCompleted(newSubscriptions);
    } else if (const auto error = std::get_if<QString>(&result)) {
        Q_EMIT importExportError(*error);
    }
}

void VideoSource::exportSubscriptions(SubscriptionListModel *model, const QString &filePath)
{
    const auto result = OPMLParser::saveOPMLSubscriptions(model->channels(), filePath);
    if (const auto error = result) {
        Q_EMIT importExportError(*error);
    } else {
        Q_EMIT importExportCompleted(model->rowCount({}));
    }
}

#include "moc_videosource.cpp"