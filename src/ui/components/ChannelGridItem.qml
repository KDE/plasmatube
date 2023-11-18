// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import Qt5Compat.GraphicalEffects

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiComponents

BaseGridItem {
    id: root

    property string name
    property string avatarUrl

    contentItem: ColumnLayout {
        id: column
        anchors.fill: parent
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding

        spacing: 0

        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: root.contextMenuRequested()
        }

        KirigamiComponents.Avatar {
            source: root.avatarUrl
            cache: true
            name: root.name

            Layout.alignment: Qt.AlignHCenter
        }

        Kirigami.Heading {
            text: root.name
            horizontalAlignment: Text.AlignHCenter

            Layout.fillWidth: true
        }
    }
}

