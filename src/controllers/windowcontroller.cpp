// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "windowcontroller.h"

#include <KConfig>
#include <KSharedConfig>
#include <KWindowConfig>

void WindowController::setWindow(QQuickWindow *window)
{
    Q_ASSERT(window != nullptr);
    m_window = window;
}

#include "moc_windowcontroller.cpp"
