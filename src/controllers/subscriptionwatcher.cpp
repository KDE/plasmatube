// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "subscriptionwatcher.h"

#include "plasmatube.h"

SubscriptionWatcher::SubscriptionWatcher(QObject *parent)
    : QObject(parent)
{
    connect(PlasmaTube::instance().selectedSource(), &VideoSource::subscriptionsChanged, this, [=] {
        if (m_channelId.isEmpty()) {
            return;
        }
        setIsSubscribed();
    });
}

QString SubscriptionWatcher::channelId() const
{
    return m_channelId;
}

void SubscriptionWatcher::setChannelId(const QString &channelId)
{
    if (m_channelId != channelId) {
        m_channelId = channelId;
        Q_EMIT channelIdChanged();

        setIsSubscribed();
    }
}

bool SubscriptionWatcher::isSubscribed() const
{
    return m_isSubscribed;
}

void SubscriptionWatcher::setIsSubscribed(bool isSubscribed)
{
    if (m_isSubscribed != isSubscribed) {
        m_isSubscribed = isSubscribed;
        Q_EMIT isSubscribedChanged();
    }
}

void SubscriptionWatcher::setIsSubscribed(std::optional<bool> isSubscribed)
{
    setIsSubscribed(isSubscribed.value_or(false));
}

void SubscriptionWatcher::setIsSubscribed()
{
    setIsSubscribed(PlasmaTube::instance().selectedSource()->isSubscribedToChannel(m_channelId));
}

#include "moc_subscriptionwatcher.cpp"