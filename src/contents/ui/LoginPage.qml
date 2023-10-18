// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube
import org.kde.plasmatube.private

FormCard.FormCardPage {
    id: root

    title: i18n("Sign in")

    data: [
        LogInController {
            id: logInController

            onLoggedIn: {
                applicationWindow().showPassiveNotification(i18n("Successfully logged in as %1.", PlasmaTube.invidiousId))
                applicationWindow().pageStack.layers.pop()
            }

            onErrorOccurred: (errorText) => {
                applicationWindow().showPassiveNotification(errorText)
            }
        },
        QQC2.BusyIndicator {
            parent: root
            anchors.centerIn: parent
            visible: logInController.isLoading
        }
    ]

    Component.onCompleted: invidiousInstance.clicked()

    FormCard.FormHeader {
        title: i18n("Log in to an Invidious Account")
        visible: !logInController.isLoading
    }

    FormCard.FormCard {
        visible: !logInController.isLoading

        FormCard.FormTextFieldDelegate {
            id: invidiousInstance
            label: i18n("Instance Url")
            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
            text: Settings !== null ? Settings.invidiousInstance : ""
            status: Kirigami.MessageType.Error

            onAccepted: usernameField.clicked()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: usernameField
            label: i18n("Username")
            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
            status: Kirigami.MessageType.Error

            onAccepted: passwordField.clicked()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: passwordField
            label: i18n("Password")
            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
            echoMode: TextInput.Password
            status: Kirigami.MessageType.Error

            onAccepted: logInButton.clicked()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: logInButton
            text: i18n("Log in")
            onClicked: {
                invidiousInstance.statusMessage = ""
                usernameField.statusMessage = ""
                passwordField.statusMessage = ""

                if (!invidiousInstance.text) {
                    invidiousInstance.statusMessage = i18n("Instance URL must not be empty!")
                    return;
                }

                if (!usernameField.text) {
                    usernameField.statusMessage = i18n("Username must not be empty!")
                    return;
                }

                if (!passwordField.text) {
                    passwordField.statusMessage = i18n("Password must not be empty!")
                    return;
                }

                logInController.logIn(usernameField.text, passwordField.text, invidiousInstance.text)
            }
        }
    }
}
