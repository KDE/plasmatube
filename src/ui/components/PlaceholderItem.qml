// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

import org.kde.kirigami as Kirigami

Rectangle {
    color: "darkgray"

    Row {
        anchors.centerIn: parent

        spacing: Kirigami.Units.mediumSpacing

        Repeater {
            model: 3

            delegate: Rectangle {
                width: 10
                height: 10
                radius: 10

                color: "gray"
            }
        }
    }
}