// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "api/abstractapi.h"
#include "videosource.h"

class LogInController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(VideoSource *source READ source WRITE setSource NOTIFY sourceChanged)

public:
    explicit LogInController(QObject *parent = nullptr);

    bool isLoading() const;

    Q_INVOKABLE void logIn(const QString &username, const QString &password);

    VideoSource *source() const;
    void setSource(VideoSource *source);

Q_SIGNALS:
    void isLoadingChanged();
    void errorOccurred(const QString &errorText);
    void loggedIn();
    void sourceChanged();

private:
    QFutureWatcher<QInvidious::LogInResult> *m_watcher = nullptr;
    VideoSource *m_source = nullptr;
};
