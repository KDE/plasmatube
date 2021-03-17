// SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10

Rectangle {
    anchors.fill: parent
    color: "transparent"
    radius: 3
    border {
        color: "white"
        width: parent.focus ? 3 : 0
    }
    Component.onCompleted: parent.activeFocusOnTab = true
    Connections {
        target: parent
        function onFocusChanged() {
            showControls = true
        }
    }
}

