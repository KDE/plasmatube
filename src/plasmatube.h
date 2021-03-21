// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

namespace QInvidious
{
    class InvidiousApi;
}

class PlasmaTube : public QObject
{
    Q_OBJECT

public:
    explicit PlasmaTube(QObject *parent = nullptr);

    static PlasmaTube &instance();

    QInvidious::InvidiousApi *api() const;

private:
    QInvidious::InvidiousApi *const m_api;
};
