/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

        onLoggingInFailed: {
            showPassiveNotification(qsTr("Couldn't log in:") + " " + errorText);
            loadingIndicator.visible = false;
            form.visible = true;
        }

        onLoggedIn: {
            pageStack.layers.pop();
            showPassiveNotification("Successfully logged in as " + AccountManager.invidiousId + ".");
        }
    }
}
