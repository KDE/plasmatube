// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Effects

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube

Kirigami.Page {
    id: root

    title: i18nc("@title:window", "Welcome")
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None

    header: Kirigami.Separator {
        width: root.width
    }

    contentItem: Item {
        ColumnLayout {
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            spacing: Kirigami.Units.largeSpacing

            Image {
                source: "qrc:/org.kde.plasmatube.svg"

                fillMode: Image.PreserveAspectFit

                Layout.fillWidth: true
            }

            Kirigami.Heading {
                text: i18nc("@info", "PlasmaTube")
                level: 1
                horizontalAlignment: Text.AlignHCenter
                font.bold: true

                Layout.fillWidth: true
            }

            QQC2.Label {
                text: i18nc("@info", "Watch YouTube and PeerTube videos.")
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap

                Layout.fillWidth: true
            }

            FormCard.FormCard {
                Layout.topMargin: Kirigami.Units.largeSpacing

                maximumWidth: Kirigami.Units.gridUnit * 16

                FormCard.FormButtonDelegate {
                    id: registerButton
                    text: i18nc("@action:button", "Add a Video Source")
                    icon.name: "network-server-symbolic"
                    onClicked: Window.window.pageStack.push(Qt.createComponent("org.kde.plasmatube", "SelectSourcePage"))
                    focus: true
                }
            }

            FormCard.FormCard {
                Layout.topMargin: Kirigami.Units.largeSpacing

                maximumWidth: Kirigami.Units.gridUnit * 16

                FormCard.FormButtonDelegate {
                    id: settingsButton
                    text: i18nc("@action:button Application settings", "Settings")
                    icon.name: "settings-configure"
                    onClicked: QQC2.ApplicationWindow.window.pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "SettingsPage"), {}, {title: i18nc("@title:window", "Settings")});
                }
            }
        }
    }
}
