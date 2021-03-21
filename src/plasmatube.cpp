// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "plasmatube.h"

#include "qinvidious/invidiousapi.h"

#include "constants.h"

PlasmaTube::PlasmaTube(QObject *parent)
    : QObject(parent),
      m_api(new QInvidious::Api(new QNetworkAccessManager(this), this))
{
    m_api->setRegion(QLocale::system().name().split("_").first());
    m_api->setCredentials(DEFAULT_INSTANCE);
}

PlasmaTube &PlasmaTube::instance()
{
    static PlasmaTube instance;
    return instance;
}

QInvidious::InvidiousApi *PlasmaTube::api() const
{
    return m_api;
}
