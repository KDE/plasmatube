// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "qinvidious/abstractapi.h"

class LogInController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    explicit LogInController(QObject *parent = nullptr);

    bool isLoading() const;

    Q_INVOKABLE void logIn(const QString &username, const QString &password, const QString &invidiousInstance);

Q_SIGNALS:
    void isLoadingChanged();
    void errorOccurred(const QString &errorText);
    void loggedIn();

private:
    QFutureWatcher<QInvidious::LogInResult> *m_watcher = nullptr;
};
