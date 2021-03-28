// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.8 as Kirigami

import org.kde.plasmatube 1.0

Kirigami.ScrollablePage {
    title: qsTr("Sign in")
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    LogInController {
        id: logInController

        onLoggedIn: {
            applicationWindow().showPassiveNotification("Successfully logged in as " + PlasmaTube.invidiousId + ".")
            pageStack.layers.pop()
        }
        onErrorOccurred: (errorText) => {
            applicationWindow().showPassiveNotification(errorText)
        }
    }

    Controls.BusyIndicator {
        anchors.centerIn: parent
        visible: logInController.isLoading
    }

    ColumnLayout {
        anchors.centerIn: parent
        visible: !logInController.isLoading

        Controls.Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Please visit the website to register with an invidious instance.\nThere is currently no API for registering.")
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        Kirigami.FormLayout {
            Controls.ComboBox {
                id: invidiousInstance
                editable: true
                model: ["https://ytprivate.com", "https://invidiou.site"]
            }

            Controls.TextField {
                id: usernameField
                placeholderText: qsTr("Username")

                onAccepted: {
                    passwordField.forceActiveFocus()
                }
            }

            Kirigami.PasswordField {
                id: passwordField

                onAccepted: {
                    logInButton.clicked()
                }
            }
        }

        Controls.Button {
            id: logInButton
            Layout.alignment: Qt.AlignRight
            text: qsTr("Sign in")
            onClicked: {
                logInController.logIn(usernameField.text, passwordField.text, invidiousInstance.editText)
            }
        }
    }
}
