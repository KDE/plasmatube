// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

QQC2.ItemDelegate {
    id: root

    signal contextMenuRequested

    hoverEnabled: !Kirigami.Settings.hasTransientTouchInput
    onPressAndHold: contextMenuRequested()
}