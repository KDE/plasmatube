// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Window

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube
import org.kde.plasmatube.private

FormCard.FormCardPage {
    id: root

    required property var source

    readonly property bool isValid: usernameField.text.length !== 0 && passwordField.text.length !== 0

    title: i18n("Sign in")

    data: [
        LogInController {
            id: logInController

            source: root.source

            onLoggedIn: {
                applicationWindow().showPassiveNotification(i18n("Successfully logged in!"))
                root.Window.window.pageStack.layers.pop()
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

    Component.onCompleted: usernameField.clicked()

    FormCard.FormHeader {
        title: i18n("Log in")
        visible: !logInController.isLoading
    }

    FormCard.FormCard {
        visible: !logInController.isLoading

        FormCard.FormTextFieldDelegate {
            id: usernameField
            label: i18n("Username")
            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
            status: Kirigami.MessageType.Error

            onAccepted: passwordField.clicked()
        }

        FormCard.FormDelegateSeparator {
            above: usernameField
            below: passwordField
        }

        FormCard.FormTextFieldDelegate {
            id: passwordField
            label: i18n("Password")
            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
            echoMode: TextInput.Password
            status: Kirigami.MessageType.Error

            onAccepted: logInButton.clicked()
        }

        FormCard.FormDelegateSeparator {
            above: passwordField
            below: logInButton
        }

        FormCard.FormButtonDelegate {
            id: logInButton
            text: i18n("Log in")
            enabled: root.isValid
            onClicked: {
                usernameField.statusMessage = ""
                passwordField.statusMessage = ""

                if (!usernameField.text) {
                    usernameField.statusMessage = i18n("Username must not be empty!")
                    return;
                }

                if (!passwordField.text) {
                    passwordField.statusMessage = i18n("Password must not be empty!")
                    return;
                }

                logInController.logIn(usernameField.text, passwordField.text)
            }
        }
    }
}
