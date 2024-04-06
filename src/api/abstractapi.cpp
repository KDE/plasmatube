// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstractapi.h"
#include <qt6keychain/keychain.h>

namespace QInvidious
{

AbstractApi::AbstractApi(QNetworkAccessManager *netManager, QObject *parent)
    : QObject(parent)
    , m_netManager(netManager)
{
}

QString AbstractApi::region() const
{
    return m_region;
}

void AbstractApi::setRegion(const QString &region)
{
    m_region = region;
}

QString AbstractApi::language() const
{
    return m_language;
}

void AbstractApi::setLanguage(const QString &language)
{
    m_language = language;
}

QNetworkAccessManager *AbstractApi::net() const
{
    return m_netManager;
}

QUrl AbstractApi::apiUrl(const QString &path) const
{
    QUrl url = QUrl::fromUserInput(m_apiHost);
    url.setScheme(QStringLiteral("https"));
    url.setPath(path);

    return url;
}

void AbstractApi::setApiHost(const QString &host)
{
    m_apiHost = host;
    Q_EMIT apiHostChanged();
}

QString AbstractApi::apiHost() const
{
    return m_apiHost;
}

std::optional<QString> AbstractApi::getKeychainValue(const QString &prefix, const QString &key)
{
    auto loop = new QEventLoop();

    auto job = new QKeychain::ReadPasswordJob(QStringLiteral("PlasmaTube"));
    job->setKey(transformKey(prefix, key));
    job->start();

    QString value;

    QObject::connect(job, &QKeychain::ReadPasswordJob::finished, [loop, job, &value](QKeychain::Job *) {
        value = job->textData();
        loop->quit();
    });

    loop->exec();

    if (!value.isEmpty()) {
        return value;
    }

    return std::nullopt;
}

void AbstractApi::setKeychainValue(const QString &prefix, const QString &key, const QString &value)
{
    auto cookieJob = new QKeychain::WritePasswordJob{QStringLiteral("PlasmaTube"), this};
    cookieJob->setKey(transformKey(prefix, key));
    cookieJob->setTextData(value);
    cookieJob->start();
}

void AbstractApi::wipeKeychainValue(const QString &prefix, const QString &key)
{
    auto cookieDeleteJob = new QKeychain::DeletePasswordJob{QStringLiteral("PlasmaTube"), this};
    cookieDeleteJob->setKey(transformKey(prefix, key));
    cookieDeleteJob->start();
}

QString AbstractApi::transformKey(const QString &prefix, const QString &key)
{
#ifdef PLASMATUBE_FLATPAK
    return QStringLiteral("%1-%2-flatpak").arg(prefix, key);
#else
    return QStringLiteral("%1-%2").arg(prefix, key);
#endif
}
}