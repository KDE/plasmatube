// SPDX-FileCopyrightText: 2022 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasmatube 1.0

Kirigami.ScrollablePage {
    title: i18n("Settings")

    Kirigami.FormLayout {
        Kirigami.Heading {
            Kirigami.FormData.isSection: true
            text: i18n("Invidious")
        }

        RowLayout {
            Kirigami.FormData.label: i18n("Instance")

            Controls.TextField {
                text: Settings !== null ? Settings.invidiousInstance : ""
                inputMethodHints: Qt.ImhUrlCharactersOnly
                onAccepted: Settings.invidiousInstance = text
            }

        }

        Kirigami.Heading {
            Kirigami.FormData.isSection: true
            text: i18n("Account")
        }

        Controls.Button {
            text: i18n("Open Account Settings")
            icon.name: "user-identity"
            onClicked: applicationWindow().pageStack.push(accountPageComponent);
        }

    }

    Component {
        id: accountPageComponent
        AccountPage {}
    }
}
