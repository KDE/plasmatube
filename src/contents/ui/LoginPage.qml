// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

import org.kde.plasmatube 1.0

Kirigami.ScrollablePage {
    title: i18n("Sign in")

    LogInController {
        id: logInController

        onLoggedIn: {
            applicationWindow().showPassiveNotification(i18n("Successfully logged in as %1.", PlasmaTube.invidiousId))
            applicationWindow().pageStack.layers.pop()
        }

        onErrorOccurred: (errorText) => {
            applicationWindow().showPassiveNotification(errorText)
        }
    }

    Controls.BusyIndicator {
        anchors.centerIn: parent
        visible: logInController.isLoading
    }

    Component.onCompleted: invidiousInstance.clicked()

    ColumnLayout {
        width: parent.width
        visible: !logInController.isLoading

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Log in to an Invidious Account")
                }

                MobileForm.FormTextFieldDelegate {
                    id: invidiousInstance
                    label: i18n("Instance Url")
                    inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
                    text: Settings !== null ? Settings.invidiousInstance : ""
                    status: Kirigami.MessageType.Error

                    onAccepted: usernameField.clicked()
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormTextFieldDelegate {
                    id: usernameField
                    label: i18n("Username")
                    inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
                    status: Kirigami.MessageType.Error

                    onAccepted: passwordField.clicked()
                }

                MobileForm.FormTextFieldDelegate {
                    id: passwordField
                    label: i18n("Password")
                    inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
                    echoMode: TextInput.Password
                    status: Kirigami.MessageType.Error

                    onAccepted: logInButton.clicked()
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormButtonDelegate {
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
    }
}
