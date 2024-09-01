// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kitemmodels as KItemModels

import org.kde.plasmatube

FormCard.FormCardPage {
    id: root

    title: i18nc("@title:window", "Add a Video Source")

    component SourceDelegate: FormCard.FormButtonDelegate {
        id: delegate

        property string websiteUrl

        contentItem: RowLayout {
            spacing: Kirigami.Units.mediumSpacing

            Kirigami.IconTitleSubtitle {
                Layout.fillWidth: true

                icon.name: delegate.icon.name
                title: delegate.text
                subtitle: delegate.description
            }

            QQC2.ToolButton {
                text: i18n("Visit Website")
                icon.name: "help-about-symbolic"
                display: QQC2.AbstractButton.IconOnly
                onClicked: Qt.openUrlExternally(delegate.websiteUrl)

                QQC2.ToolTip.text: text
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                QQC2.ToolTip.visible: hovered
            }

            FormCard.FormArrow {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                direction: Qt.RightArrow
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        SourceDelegate {
            id: invidiousButton

            text: i18n("Add Invidious Source")
            description: i18n("Watch YouTube videos")
            icon.name: "plasmatube-invidious"
            websiteUrl: "https://invidious.io/"

            onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "AddInvidiousPage"))
        }

        FormCard.FormDelegateSeparator {
            above: invidiousButton
            below: peerTubeButton
        }

        SourceDelegate {
            id: peerTubeButton

            text: i18n("Add PeerTube Source")
            description: i18n("Watch PeerTube videos")
            icon.name: "plasmatube-peertube"
            websiteUrl: "https://joinpeertube.org/"

            onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "AddPeerTubePage"))
        }

        FormCard.FormDelegateSeparator {
            above: peerTubeButton
            below: pipedButton
        }

        SourceDelegate {
            id: pipedButton

            text: i18n("Add Piped Source")
            description: i18n("Watch YouTube videos")
            icon.name: "plasmatube-piped"
            websiteUrl: "https://github.com/TeamPiped/Piped"

            onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "AddPipedPage"))
        }
    }
}
