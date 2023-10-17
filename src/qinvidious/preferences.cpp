// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "preferences.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

Preferences Preferences::fromJson(const QJsonObject &obj, Preferences &preferences)
{
    preferences.setAutoPlay(obj["autoplay"_L1].toBool());
    preferences.m_originalPreferences = obj;
    return preferences;
}

QJsonObject Preferences::toJson() const
{
    QJsonObject obj = m_originalPreferences;
    obj["autoplay"_L1] = autoPlay();

    return obj;
}

bool Preferences::autoPlay() const
{
    return m_autoPlay;
}

void Preferences::setAutoPlay(bool autoPlay)
{
    m_autoPlay = autoPlay;
}

QString Preferences::defaultHome() const
{
    return m_defaultHome;
}

void Preferences::setDefaultHome(const QString &defaultHome)
{
    m_defaultHome = defaultHome;
}
