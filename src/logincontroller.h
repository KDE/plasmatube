// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "qinvidious/invidiousapi.h"

class LogInController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    explicit LogInController(QObject *parent = nullptr);

    bool isLoading() const;
    Q_SIGNAL void isLoadingChanged();

    Q_INVOKABLE void logIn(const QString &username, const QString &password, const QString &invidiousInstance);

    Q_SIGNAL void errorOccurred(const QString &errorText);
    Q_SIGNAL void loggedIn();

private:
    QFutureWatcher<QInvidious::LogInResult> *m_watcher = nullptr;
};
