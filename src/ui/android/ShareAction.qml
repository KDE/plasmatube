// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2
import org.kde.kirigami 2 as Kirigami

/**
 * Android (currently no-op version) of ShareAction.
 */
Kirigami.Action {
    id: shareAction
    icon.name: "emblem-shared-symbolic"
    text: i18n("Share")
    tooltip: i18n("Share the selected media")

    visible: false

    property var inputData: ({})
}