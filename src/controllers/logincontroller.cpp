// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logincontroller.h"
#include "controllers/plasmatube.h"

#include <KLocalizedString>
#include <QFutureWatcher>

LogInController::LogInController(QObject *parent)
    : QObject(parent)
{
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

    auto future = m_source->api()->logIn(username, password);

    m_watcher = new QFutureWatcher<QInvidious::LogInResult>(this);
    connect(m_watcher, &QFutureWatcherBase::finished, this, [=] {
        auto result = m_watcher->result();

        if (const auto credentials = std::get_if<QInvidious::Credentials>(&result)) {
            m_source->setUsername(username);
            m_source->setCookie(QString::fromUtf8(credentials->cookie()->toRawForm()));
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
        }

        m_watcher->deleteLater();
        m_watcher = nullptr;
        Q_EMIT isLoadingChanged();
    });
    m_watcher->setFuture(future);

    Q_EMIT isLoadingChanged();
}

VideoSource *LogInController::source() const
{
    return m_source;
}

void LogInController::setSource(VideoSource *source)
{
    if (source != m_source) {
        m_source = source;
        Q_EMIT sourceChanged();
    }
}

#include "moc_logincontroller.cpp"