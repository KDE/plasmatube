// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractapi.h"
#include "channel.h"
#include "videosource.h"
#include <QAbstractListModel>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QtQml>

class SubscriptionListModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum CustomRoles { NameRole = Qt::UserRole, ChannelIdRole, AvatarRole, SubCountRole };

    explicit SubscriptionListModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void fill(VideoSource *source);

Q_SIGNALS:
    void loadingChanged();

private:
    QList<QInvidious::Channel> m_channels;
    bool m_loading = false;

    // history
    QFutureSynchronizer<QInvidious::ChannelResult> m_historyFutureSync;
    QFutureWatcher<void> *m_historyFetchFinishWatcher = nullptr;
};
