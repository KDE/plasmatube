// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtQml/qqmlregistration.h>

#include "abstractapi.h"
#include "subscriptionwatcher.h"

template<typename T>
class QFutureWatcher;

class SubscriptionController : public SubscriptionWatcher
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    explicit SubscriptionController(QObject *parent = nullptr);

    bool isLoading() const;

    Q_INVOKABLE bool canToggleSubscription() const;
    Q_INVOKABLE void toggleSubscription();

Q_SIGNALS:
    void isLoadingChanged();
    void errorOccurred(const QString &errorText);

private:
    QFutureWatcher<QInvidious::Result> *m_watcher;
};

