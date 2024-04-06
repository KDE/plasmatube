// SPDX-FileCopyrightText: 2014 Sujith Haridasan <sujith.haridasan@kdemail.net>
// SPDX-FileCopyrightText: 2014 Ashish Madeti <ashishmadeti@gmail.com>
// SPDX-FileCopyrightText: 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
// SPDX-FileCopyrightText: 2022-2023 Bart De Vries <bart@mogwai.be>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mediaplayer2.h"

#include "mpris2_logging.h"

#include <KAboutData>

#include <QCoreApplication>
#include <QDebug>

MediaPlayer2::MediaPlayer2(VideoController *audioPlayer, QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_audioPlayer(audioPlayer)
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::MediaPlayer2()";

    // TODO: implement
    // connect(this, &MediaPlayer2::raisePlayer, audioPlayer, &VideoController::raiseWindowRequested);
    // connect(this, &MediaPlayer2::quitPlayer, audioPlayer, &VideoController::quitRequested);
}

MediaPlayer2::~MediaPlayer2()
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::~MediaPlayer2()";
}

bool MediaPlayer2::CanQuit() const
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::CanQuit()";
    return true;
}

bool MediaPlayer2::CanRaise() const
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::CanRaise()";
    return true;
}
bool MediaPlayer2::HasTrackList() const
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::HasTrackList()";
    return false;
}

void MediaPlayer2::Quit()
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::Quit()";
    Q_EMIT quitPlayer();
}

void MediaPlayer2::Raise()
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::Raise()";
    Q_EMIT raisePlayer();
}

QString MediaPlayer2::Identity() const
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::Identity()";
    return QStringLiteral("PlasmaTube");
}

QString MediaPlayer2::DesktopEntry() const
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::DesktopEntry()";
    return QLatin1String("org.kde.plasmatube");
}

QStringList MediaPlayer2::SupportedUriSchemes() const
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::SupportedUriSchemes()";
    return QStringList() << QStringLiteral("file");
}

QStringList MediaPlayer2::SupportedMimeTypes() const
{
    qCDebug(PLASMATUBE_MPRIS2) << "MediaPlayer2::SupportedMimeTypes()";
    return {};
}
