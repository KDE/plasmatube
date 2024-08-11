// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "plasmatube.h"

#include "config.h"

#include <QNetworkProxy>
#include <QSettings>
#include <QStringBuilder>

PlasmaTube::PlasmaTube(QObject *parent)
    : QObject(parent)
    , m_controller(new VideoController(this))
    , m_sourceManager(new SourceManager(this))
    , m_networkAccessManager(new QNetworkAccessManager(this))
    , m_settings(KSharedConfig::openConfig(QStringLiteral("plasmatuberc"), KConfig::SimpleConfig, QStandardPaths::AppConfigLocation))
{
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, &m_settings, &Settings::save);
    setApplicationProxy();
    m_sourceManager->load();
    connect(m_sourceManager, &SourceManager::sourceSelected, this, &PlasmaTube::sourceSelected);
}

PlasmaTube &PlasmaTube::instance()
{
    static PlasmaTube instance;
    return instance;
}

VideoController *PlasmaTube::videoController() const
{
    return m_controller;
}

SourceManager *PlasmaTube::sourceManager() const
{
    return m_sourceManager;
}

VideoSource *PlasmaTube::selectedSource() const
{
    return m_sourceManager->selectedSource();
}

QNetworkAccessManager *PlasmaTube::networkAccessManager() const
{
    return m_networkAccessManager;
}

void PlasmaTube::setApplicationProxy()
{
    Settings settings(KSharedConfig::openConfig(QStringLiteral("plasmatuberc"), KConfig::SimpleConfig, QStandardPaths::AppConfigLocation));
    QNetworkProxy proxy;

    // type match to ProxyType from config.kcfg
    switch (settings.proxyType()) {
    case 1: // HTTP
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(settings.proxyHost());
        proxy.setPort(settings.proxyPort());
        proxy.setUser(settings.proxyUser());
        proxy.setPassword(settings.proxyPassword());
        break;
    case 0: // System Default
    default:
        // do nothing
        break;
    }

    QNetworkProxy::setApplicationProxy(proxy);
}

Settings *PlasmaTube::settings()
{
    return &m_settings;
}

void PlasmaTube::saveCurrentSource()
{
    auto stateConfig = KSharedConfig::openStateConfig();
    auto generalGroup = stateConfig->group(QStringLiteral("General"));

    generalGroup.writeEntry(QStringLiteral("LastSource"), m_sourceManager->selectedSource()->uuid());

    stateConfig->sync();
}

#include "moc_plasmatube.cpp"