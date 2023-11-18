// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qinvidiousglobal.h"

namespace QInvidious
{

class Preferences
{
    Q_GADGET

    Q_PROPERTY(bool autoPlay MEMBER m_autoPlay)
    Q_PROPERTY(QString defaultHome MEMBER m_defaultHome)

public:
    FROM_JSON_OVERLOADS(Preferences)
    static Preferences fromJson(const QJsonObject &, Preferences &);
    [[nodiscard]] QJsonObject toJson() const;

    bool autoPlay() const;
    void setAutoPlay(bool autoPlay);

    QString defaultHome() const;
    void setDefaultHome(const QString &defaultHome);

private:
    bool m_autoPlay = false;
    QString m_defaultHome;
    QJsonObject m_originalPreferences; // We need to store all the preferences, to not overwrite them back to default
};

}
Q_DECLARE_METATYPE(QInvidious::Preferences)
