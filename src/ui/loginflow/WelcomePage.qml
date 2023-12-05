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

    Kirigami.Icon {
        source: "org.kde.plasmatube"

        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: Kirigami.Units.largeSpacing
        implicitWidth: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
        implicitHeight: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
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
            icon.name: "plasmatube-invidious"
            onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "AddInvidiousPage"))
        }

        FormCard.FormDelegateSeparator {
            above: invidiousButton
            below: peerTubeButton
        }

        FormCard.FormButtonDelegate {
            id: peerTubeButton
            text: i18n("Add PeerTube Source")
            icon.name: "plasmatube-peertube"
            onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "AddPeerTubePage"))
        }

        FormCard.FormDelegateSeparator {
            above: peerTubeButton
            below: pipedButton
        }

        FormCard.FormButtonDelegate {
            id: pipedButton
            text: i18n("Add Piped Source")
            icon.name: "plasmatube-piped"
            onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "AddPipedPage"))
        }
    }
}
