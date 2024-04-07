// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "subscriptionlistmodel.h"

#include "plasmatube.h"
#include "videosource.h"

#include <QUrlQuery>
#include <QtConcurrent>

using namespace Qt::Literals::StringLiterals;

SubscriptionListModel::SubscriptionListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant SubscriptionListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    auto &channel = m_channels[index.row()];

    switch (role) {
    case NameRole:
        return channel.name();
    case ChannelIdRole:
        return channel.id();
    case AvatarRole:
        return channel.avatar();
    case SubCountRole:
        return channel.subCount();
    default:
        return {};
    }
}

bool SubscriptionListModel::loading() const
{
    return m_loading;
}

void SubscriptionListModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int SubscriptionListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_channels.size());
}

QHash<int, QByteArray> SubscriptionListModel::roleNames() const
{
    return {{NameRole, "name"}, {ChannelIdRole, "channelId"}, {AvatarRole, "avatar"}, {SubCountRole, "subCount"}};
}

void SubscriptionListModel::fill(VideoSource *source)
{
    if (m_loading) {
        return;
    }
    setLoading(true);

    for (const auto &channelId : *source->subscriptions()) {
        auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestChannelInfo(channelId);
        m_historyFutureSync.addFuture(future);
    }

    m_historyFetchFinishWatcher = new QFutureWatcher<void>();
    m_historyFetchFinishWatcher->setFuture(QtConcurrent::run([this] {
        m_historyFutureSync.waitForFinished();
    }));

    connect(m_historyFetchFinishWatcher, &QFutureWatcherBase::finished, this, [this] {
        for (const auto &future : m_historyFutureSync.futures()) {
            auto result = future.result();
            if (auto video = std::get_if<QInvidious::Channel>(&result)) {
                const auto rows = rowCount({});
                beginInsertRows({}, rows, rows);
                m_channels.push_back(*video);
                endInsertRows();
            } else {
                // TODO: report error
            }
        }

        m_historyFutureSync.clearFutures();

        m_historyFetchFinishWatcher->deleteLater();
        m_historyFetchFinishWatcher = nullptr;
        setLoading(false);
    });
}

QList<QInvidious::Channel> SubscriptionListModel::channels() const
{
    return m_channels;
}

#include "moc_subscriptionlistmodel.cpp"