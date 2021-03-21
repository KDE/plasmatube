// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logincontroller.h"

#include <QFutureWatcher>

#include "plasmatube.h"

LogInController::LogInController(QObject *parent)
    : QObject(parent)
{
    connect(this, &LogInController::loggedIn, &PlasmaTube::instance(), &PlasmaTube::loggedIn);
}

bool LogInController::isLoading() const
{
    return m_watcher != nullptr;
}

void LogInController::logIn(const QString &username, const QString &password)
{
    if (m_watcher) {
        m_watcher->cancel();
        m_watcher->deleteLater();
        m_watcher = nullptr;
    }

    auto future = PlasmaTube::instance().api()->logIn(username, password);

    m_watcher = new QFutureWatcher<QInvidious::LogInResult>(this);
    connect(m_watcher, &QFutureWatcherBase::finished, this, [=] {
        auto result = m_watcher->result();

        if (const auto crendentials = std::get_if<QInvidious::Credentials>(&result)) {
            // credentials are set automatically
            PlasmaTube::instance().saveCredentials();
            emit loggedIn();
        } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
            switch (error->first) {
            case QNetworkReply::AuthenticationRequiredError:
                emit errorOccurred(tr("Username or password is wrong."));
                break;
            case QNetworkReply::ContentAccessDenied:
                emit errorOccurred(tr("This instance has disabled the registration."));
                break;
            default:
                emit errorOccurred(error->second);
            }
        }

        m_watcher->deleteLater();
        m_watcher = nullptr;
        emit isLoadingChanged();
    });
    m_watcher->setFuture(future);

    emit isLoadingChanged();
}
