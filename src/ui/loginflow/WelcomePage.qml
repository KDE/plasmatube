// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardPage {
    id: root

    title: i18nc("@title:window", "Welcome")

    Image {
        source: "qrc:/org.kde.plasmatube.svg"

        fillMode: Image.PreserveAspectFit

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: Kirigami.Units.largeSpacing * 3
    }

    FormCard.FormHeader {
        title: i18n("Welcome to PlasmaTube")
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            id: learnMoreDelegate
            text: i18n("PlasmaTube requires at least one video source.")
        }

        FormCard.FormDelegateSeparator {
            above: learnMoreDelegate
            below: invidiousButton
        }

        FormCard.FormButtonDelegate {
            id: invidiousButton
            text: i18n("Add Invidious Source")
            onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "AddInvidiousPage"))
        }

        FormCard.FormDelegateSeparator {
            above: invidiousButton
            below: peerTubeButton
        }

        FormCard.FormButtonDelegate {
            id: peerTubeButton
            text: i18n("Add PeerTube Source")
            onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "AddPeerTubePage"))
        }
    }
}
