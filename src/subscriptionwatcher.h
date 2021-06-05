// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <optional>

class SubscriptionWatcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString channelId READ channelId WRITE setChannelId NOTIFY channelIdChanged)
    Q_PROPERTY(bool isSubscribed READ isSubscribed NOTIFY isSubscribedChanged)

public:
    explicit SubscriptionWatcher(QObject *parent = nullptr);

    QString channelId() const;
    void setChannelId(const QString &channelId);
    Q_SIGNAL void channelIdChanged();

    bool isSubscribed() const;
    Q_SIGNAL void isSubscribedChanged();

protected:
    void setIsSubscribed(bool isSubscribed);
    void setIsSubscribed(std::optional<bool> isSubscribed);
    void setIsSubscribed();

private:
    QString m_channelId;
    bool m_isSubscribed = false;
};
