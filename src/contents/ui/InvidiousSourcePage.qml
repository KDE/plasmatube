// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Window
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardPage {
    id: page

    required property var source

    title: i18nc("@title:window", "Edit Source")

    FormCard.FormHeader {
        title: i18n("Account")
    }

    FormCard.FormCard {
        QQC2.Label {
            id: isLoggedInLabel

            visible: page.source.loggedIn
            Layout.fillWidth: true
            text: i18n("Currently logged in as %1.", page.source.username)
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            padding: Kirigami.Units.largeSpacing
        }

        FormCard.FormButtonDelegate {
            visible: !page.source.loggedIn
            Layout.alignment: Qt.AlignHCenter
            text: i18n("Log in")
            onClicked: pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "LoginPage"), {source: page.source});
        }

        FormCard.FormButtonDelegate {
            visible: page.source.loggedIn
            Layout.alignment: Qt.AlignHCenter
            text: i18n("Log out")
            onClicked: PlasmaTube.logOut();
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.fillWidth: true

        FormCard.FormButtonDelegate {
            text: i18n("Remove Source")
            description: !enabled ? i18n("Cannot remove the only source.") : ""
            icon.name: "delete"

            Kirigami.PromptDialog {
                id: deletePrompt

                title: i18nc("@title", "Remove Source")
                subtitle: i18nc("@label", "Are you sure you want to remove this source?")
                standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
                showCloseButton: false
            }

            onClicked: deletePrompt.open()
        }
    }
}