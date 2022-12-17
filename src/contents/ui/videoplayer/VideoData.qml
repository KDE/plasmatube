// SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Layouts 1.10
import QtQuick.Window 2.15
import QtQuick.Controls 2.10 as QQC2
import org.kde.kirigami 2.10 as Kirigami

Item {
    required property string title
    signal toggleDrawer()

    anchors.fill: parent
    Rectangle {
        height: parent.children[1].height * 3
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        gradient: Gradient {
            GradientStop { position: 1.0; color: Qt.rgba(18/255, 18/255, 18/255, 0.0) }
            GradientStop { position: 0.0; color: Qt.rgba(18/255, 18/255, 10/255, 0.8) }
        }
    }
    ColumnLayout {
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.textColor: "white"
        Kirigami.Theme.disabledTextColor: Qt.rgba(200/255, 200/255, 200/255, 0.8)
        anchors {
            top: parent.top
            left: parent.left
            leftMargin: Kirigami.Units.largeSpacing
            right: parent.right
            rightMargin: Kirigami.Units.largeSpacing
        }
        Item { height: Kirigami.Units.largeSpacing }
        Item {
            Layout.fillWidth: true
            implicitHeight: childrenRect.height
            Kirigami.Heading {
                anchors.horizontalCenter: parent.horizontalCenter
                text: title // todo also handle metadata title
                level: 3
            }
            QQC2.ToolButton {
                id: fullscreen
                anchors.right: parent.right
                text: applicationWindow().visibility != Window.FullScreen ?
                      i18n("Fullscreen") : i18n("Restore")

                icon.name: "view-fullscreen"

                checkable: true
                checked: false
                //onToggled: //Hologram.Video.fullScreen = !Hologram.Video.fullScreen

                TabIndicator {}
            }
        }
    }
}

