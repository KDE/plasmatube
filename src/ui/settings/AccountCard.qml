// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Window
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.plasmatube

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

    FormCard.FormDelegateSeparator {
        above: isLoggedInLabel
        below: loginButton
        visible: page.source.loggedIn
    }

    FormCard.FormButtonDelegate {
        id: loginButton

        text: {
            if (page.source.loggedIn) {
                return i18nc("@action:button", "Log out")
            } else {
                return i18nc("@action:button", "Log in")
            }
        }
        icon.name: page.source.loggedIn ? "go-previous-symbolic" : "im-user"
        description: !page.source.loggedIn ? i18n("Subscribe to channels, keep track of watched videos and more.") : ""
        onClicked: {
            if (page.source.loggedIn) {
                page.source.logOut();
            } else {
                page.Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "LoginPage"), {source: page.source});
            }
        }
    }
}