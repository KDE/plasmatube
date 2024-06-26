// SPDX-FileCopyrightText: 2014 Sujith Haridasan <sujith.haridasan@kdemail.net>
// SPDX-FileCopyrightText: 2014 Ashish Madeti <ashishmadeti@gmail.com>
// SPDX-FileCopyrightText: 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
// SPDX-FileCopyrightText: 2022-2023 Bart De Vries <bart@mogwai.be>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mpris2.h"

#include "mpris2_logging.h"
#include "videocontroller.h"

#if !defined Q_OS_ANDROID
#include <QDBusConnection>
#endif

#if defined Q_OS_WIN
#include <Windows.h>
#else
#include <unistd.h>
#endif

#if !defined Q_OS_ANDROID
#include "mediaplayer2.h"
#include "mediaplayer2player.h"
#endif

Mpris2::Mpris2(QObject *parent)
    : QObject(parent)
    , m_audioPlayer(dynamic_cast<VideoController *>(parent))
{
    qCDebug(PLASMATUBE_MPRIS2) << "Mpris2::Mpris2()";

#if !defined Q_OS_ANDROID
    initDBusService();
#endif
}

void Mpris2::initDBusService()
{
    qCDebug(PLASMATUBE_MPRIS2) << "Mpris2::initDBusService()";
#if !defined Q_OS_ANDROID

    QString tryPlayerName = QStringLiteral("PlasmaTube");
    QString mpris2Name(QStringLiteral("org.mpris.MediaPlayer2.") + tryPlayerName);

    bool success = QDBusConnection::sessionBus().registerService(mpris2Name);

    // If the above failed, it's likely because we're not the first instance
    // or the name is already taken. In that event the MPRIS2 spec wants the
    // following:
    if (!success) {
#if defined Q_OS_WIN
        tryPlayerName = tryPlayerName + QLatin1String("".instance) + QString::number(GetCurrentProcessId());
#else
        tryPlayerName = tryPlayerName + QLatin1String(".instance") + QString::number(getpid());
#endif
       success = QDBusConnection::sessionBus().registerService(QStringLiteral("org.mpris.MediaPlayer2.") + tryPlayerName);
    }

    if (success) {
        m_playerName = tryPlayerName;
        if (!m_mp2) {
            m_mp2 = std::make_unique<MediaPlayer2>(m_audioPlayer, this);
            m_mp2p = std::make_unique<MediaPlayer2Player>(m_audioPlayer, this);
        }

        QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this, QDBusConnection::ExportAdaptors);
        qCDebug(PLASMATUBE_MPRIS2) << "Mpris2::initDBusService() success";
    }
#endif
}

bool Mpris2::unregisterDBusService()
{
    bool success = true;
#if !defined Q_OS_ANDROID
    QString oldMpris2Name(QStringLiteral("org.mpris.MediaPlayer2.") + m_playerName);
    success = QDBusConnection::sessionBus().unregisterService(oldMpris2Name);

    if (success) {
        m_playerName = QLatin1String("");
    }
#endif
    return success;
}

Mpris2::~Mpris2()
{
    qCDebug(PLASMATUBE_MPRIS2) << "Mpris2::~Mpris2()";
    unregisterDBusService();
}
