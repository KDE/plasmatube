// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <invidious/invidiousapi.h>

template<typename T>
class QFutureWatcher;

class ChannelController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString name READ name NOTIFY channelLoaded)
    Q_PROPERTY(QString banner READ banner NOTIFY channelLoaded)
    Q_PROPERTY(QString avatar READ avatar NOTIFY channelLoaded)
    Q_PROPERTY(QString description READ description NOTIFY channelLoaded)
    Q_PROPERTY(int subCount READ subCount NOTIFY channelLoaded)
    Q_PROPERTY(QString url READ url NOTIFY channelLoaded)

public:
    explicit ChannelController(QObject *parent = nullptr);

    Q_INVOKABLE void loadChannel(const QString &channelId);

    QString name() const;
    QString banner() const;
    QString avatar() const;
    QString description() const;
    int subCount() const;
    QString url() const;

Q_SIGNALS:
    void channelLoaded();

private:
    QFutureWatcher<QInvidious::ChannelResult> *m_watcher = nullptr;
    QInvidious::Channel m_channel;
};
