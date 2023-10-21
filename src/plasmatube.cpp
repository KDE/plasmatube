// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "plasmatube.h"

#include <KLocalizedString>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QSettings>
#include <QStringBuilder>

PlasmaTube::PlasmaTube(QObject *parent)
    : QObject(parent)
    , m_controller(new VideoController(this))
    , m_sourceManager(new SourceManager(this))
{
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

VideoSource *PlasmaTube::selectedSource()
{
    return m_sourceManager->selectedSource();
}

void PlasmaTube::setInhibitSleep(const bool inhibit)
{
#ifdef HAS_DBUS
    if (inhibit) {
        QDBusMessage message = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                              QStringLiteral("/ScreenSaver"),
                                                              QStringLiteral("org.freedesktop.ScreenSaver"),
                                                              QStringLiteral("Inhibit"));
        message << QGuiApplication::desktopFileName();
        message << i18n("Playing video");

        QDBusReply<uint> reply = QDBusConnection::sessionBus().call(message);
        if (reply.isValid()) {
            screenSaverDbusCookie = reply.value();
        }
    } else {
        if (screenSaverDbusCookie != 0) {
            QDBusMessage message = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                                  QStringLiteral("/ScreenSaver"),
                                                                  QStringLiteral("org.freedesktop.ScreenSaver"),
                                                                  QStringLiteral("UnInhibit"));
            message << static_cast<uint>(screenSaverDbusCookie);
            screenSaverDbusCookie = 0;
            QDBusConnection::sessionBus().send(message);
        }
    }
#endif
}
