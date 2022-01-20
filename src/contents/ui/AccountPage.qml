// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasmatube 1.0

Kirigami.ScrollablePage {
    title: qsTr("Sign in")
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    LogInController {
        id: logInController

        onLoggedIn: {
            applicationWindow().showPassiveNotification(i18n("Successfully logged in as %1.", PlasmaTube.invidiousId))
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
        visible: !logInController.isLoading

        // components while logged in
        Controls.Label {
            visible: PlasmaTube.isLoggedIn
            Layout.fillWidth: true
            text: i18n("Currently logged in as %1.", PlasmaTube.invidiousId)
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }
        
        Controls.Button {
            visible: PlasmaTube.isLoggedIn
            Layout.alignment: Qt.AlignHCenter
            id: logOutButton
            text: i18n("Log out")
            onClicked: {
                PlasmaTube.logOut();
            }
        }
        
        // components while logged out
        Controls.Label {
            visible: !PlasmaTube.isLoggedIn
            Layout.fillWidth: true
            text: i18n("Please visit the website to register with an invidious instance.\nThere is currently no API for registering.")
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        Kirigami.FormLayout {
            visible: !PlasmaTube.isLoggedIn
            Layout.fillWidth: true
            
            Controls.ComboBox {
                id: invidiousInstance
                editable: true
                model: ["https://ytprivate.com", "https://invidiou.site"]
            }

            Controls.TextField {
                id: usernameField
                placeholderText: i18n("Username")

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
            
            Controls.Button {
                id: logInButton
                text: i18n("Sign in")
                onClicked: {
                    logInController.logIn(usernameField.text, passwordField.text, invidiousInstance.editText)
                }
            }
        }
    }
}
