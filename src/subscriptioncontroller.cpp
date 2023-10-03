// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "subscriptioncontroller.h"

#include <QFutureWatcher>

#include "qinvidious/invidiousapi.h"

#include "plasmatube.h"

SubscriptionController::SubscriptionController(QObject *parent)
    : SubscriptionWatcher(parent),
      m_watcher(nullptr)
{
}

bool SubscriptionController::isLoading() const
{
    return m_watcher != nullptr;
}

bool SubscriptionController::canToggleSubscription() const
{
    // subscriptions need to be loaded and no other task needs to run
    return PlasmaTube::instance().isLoggedIn() &&
           PlasmaTube::instance().subscriptions().has_value() &&
           !m_watcher;
}

void SubscriptionController::toggleSubscription()
{
    if (m_watcher) {
        m_watcher->cancel();
        m_watcher->deleteLater();
        m_watcher = nullptr;
    }

    auto future = isSubscribed() ? PlasmaTube::instance().api()->unsubscribeFromChannel(channelId())
                                 : PlasmaTube::instance().api()->subscribeToChannel(channelId());

    m_watcher = new QFutureWatcher<QInvidious::Result>(this);
    connect(m_watcher, &QFutureWatcherBase::finished, this, [=] {
        auto result = m_watcher->result();
        if (std::holds_alternative<QInvidious::Success>(result)) {
            auto &subs = PlasmaTube::instance().subscriptions();
            if (subs.has_value()) {
                if (isSubscribed()) {
                    // was subscribed -> now unsubscribed
                    subs->removeAll(channelId());
                } else {
                    // was not subscribed -> now subscribed
                    subs->append(channelId());
                }
                Q_EMIT PlasmaTube::instance().subscriptionsChanged();
            } else {
                qDebug() << "SubscriptionController::toggleSubscription():"
                         << "(Un)subscription successful, but subscriptions are not loaded.";
                setIsSubscribed(!isSubscribed());
            }
        } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
            Q_EMIT errorOccurred(error->second);
        }

        m_watcher->deleteLater();
        m_watcher = nullptr;
        Q_EMIT isLoadingChanged();
    });
    Q_EMIT isLoadingChanged();
    m_watcher->setFuture(future);
}
