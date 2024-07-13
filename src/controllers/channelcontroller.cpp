// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-or-later

#include "channelcontroller.h"

#include "plasmatube.h"

#include <QFutureWatcher>

ChannelController::ChannelController(QObject *parent)
    : QObject(parent)
{
}

void ChannelController::loadChannel(const QString &channelId)
{
    m_watcher = new QFutureWatcher<QInvidious::ChannelResult>();

    auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestChannelInfo(channelId);
    m_watcher->setFuture(future);

    connect(m_watcher, &QFutureWatcherBase::finished, this, [this] {
        auto result = m_watcher->result();
        if (auto channel = std::get_if<QInvidious::Channel>(&result)) {
            m_channel = *channel;
            Q_EMIT channelLoaded();
        } else if (auto error = std::get_if<QInvidious::Error>(&result)) {
            qWarning() << "Failed to fetch channel:" << error->second;
        }

        m_watcher->deleteLater();
        m_watcher = nullptr;
    });
}

QString ChannelController::name() const
{
    return m_channel.name();
}

QString ChannelController::avatar() const
{
    return m_channel.avatar();
}

QString ChannelController::banner() const
{
    return m_channel.banner();
}

QString ChannelController::description() const
{
    return m_channel.description();
}

int ChannelController::subCount() const
{
    return m_channel.subCount();
}

#include "moc_channelcontroller.cpp"