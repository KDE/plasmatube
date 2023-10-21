// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logincontroller.h"
#include "plasmatube.h"

#include <QFutureWatcher>
#include <KLocalizedString>

LogInController::LogInController(QObject *parent)
    : QObject(parent)
{
    connect(this, &LogInController::loggedIn, &PlasmaTube::instance(), &PlasmaTube::loggedIn);
}

bool LogInController::isLoading() const
{
    return m_watcher != nullptr;
}

void LogInController::logIn(const QString &username, const QString &password, const QString &invidiousInstance)
{
    if (m_watcher) {
        m_watcher->cancel();
        m_watcher->deleteLater();
        m_watcher = nullptr;
    }

    // const auto previousInstance = PlasmaTube::instance().sourceManager()->selectedSource()->api()->invidiousInstance();

    PlasmaTube::instance().sourceManager()->selectedSource()->api()->setCredentials(invidiousInstance);
    auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->logIn(username, password);

    m_watcher = new QFutureWatcher<QInvidious::LogInResult>(this);
    connect(m_watcher, &QFutureWatcherBase::finished, this, [=] {
        auto result = m_watcher->result();

        if (const auto crendentials = std::get_if<QInvidious::Credentials>(&result)) {
            // credentials are set automatically
            PlasmaTube::instance().saveCredentials();
            Q_EMIT loggedIn();
        } else if (const auto error = std::get_if<QInvidious::Error>(&result)) {
            switch (error->first) {
            case QNetworkReply::AuthenticationRequiredError:
                Q_EMIT errorOccurred(i18n("Username or password is wrong."));
                break;
            case QNetworkReply::ContentAccessDenied:
                Q_EMIT errorOccurred(i18n("This instance has disabled the registration."));
                break;
            default:
                Q_EMIT errorOccurred(error->second);
            }

            // PlasmaTube::instance().sourceManager()->selectedSource()->api()->setCredentials(previousInstance);
        }

        m_watcher->deleteLater();
        m_watcher = nullptr;
        Q_EMIT isLoadingChanged();
    });
    m_watcher->setFuture(future);

    Q_EMIT isLoadingChanged();
}
