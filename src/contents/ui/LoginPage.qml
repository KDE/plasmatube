// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.8 as Kirigami

import org.kde.plasmatube.accountmanager 1.0

Kirigami.ScrollablePage {
    title: qsTr("Sign in")
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    Controls.BusyIndicator {
        anchors.centerIn: parent
        id: loadingIndicator
        visible: false
    }

    ColumnLayout {
        id: form
        anchors.centerIn: parent

        Controls.Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Please visit the website to register with an invidious instance. There is currently no API for registering.")
            wrapMode: Text.WordWrap
        }

        Kirigami.FormLayout {
            Controls.TextField {
                id: usernameField
                placeholderText: qsTr("Username")
            }

            Kirigami.PasswordField {
                id: passwordField
            }
        }

        Controls.Button {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Sign in")
            onClicked: {
                form.visible = false;
                loadingIndicator.visible = true;
                AccountManager.logIn(usernameField.text, passwordField.text);
            }
        }
    }

    Connections {
        target: AccountManager

        function onLoggingInFailed(errorText) {
            showPassiveNotification(qsTr("Couldn't log in:") + " " + errorText);
            loadingIndicator.visible = false;
            form.visible = true;
        }

        function onLoggedIn() {
            pageStack.layers.pop();
            showPassiveNotification("Successfully logged in as " + AccountManager.invidiousId + ".");
        }
    }
}
