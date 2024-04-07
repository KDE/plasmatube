// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Window
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCard {
    id: root

    required property var source

    QQC2.Label {
        id: isLoggedInLabel

        visible: root.source.loggedIn
        Layout.fillWidth: true
        text: i18n("Currently logged in as %1.", root.source.username)
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        padding: Kirigami.Units.largeSpacing
    }

    FormCard.FormDelegateSeparator {
        above: isLoggedInLabel
        below: loginButton
        visible: root.source.loggedIn
    }

    FormCard.FormButtonDelegate {
        id: loginButton

        text: {
            if (root.source.loggedIn) {
                return i18nc("@action:button", "Log out")
            } else {
                return i18nc("@action:button", "Log in")
            }
        }
        icon.name: root.source.loggedIn ? "go-previous-symbolic" : "im-user"
        description: !root.source.loggedIn ? i18n("Subscribe to channels, keep track of watched videos and more.") : ""
        onClicked: {
            if (root.source.loggedIn) {
                root.source.logOut();
            } else {
                page.Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "LoginPage"), {source: root.source});
            }
        }
    }
}