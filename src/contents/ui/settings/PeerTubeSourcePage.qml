// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Window
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.plasmatube

FormCard.FormCardPage {
    id: page

    required property var source

    title: i18nc("@title:window", "Edit Source")

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.fillWidth: true

        FormCard.FormButtonDelegate {
            text: i18n("Remove Source")
            description: !enabled ? i18n("Cannot remove the only source.") : ""
            icon.name: "delete"
            enabled: PlasmaTube.sourceManager.canRemove()

            Kirigami.PromptDialog {
                id: deletePrompt

                title: i18nc("@title", "Remove Source")
                subtitle: i18nc("@label", "Are you sure you want to remove this source?")
                standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
                showCloseButton: false

                onAccepted: {
                    PlasmaTube.sourceManager.removeSource(page.source);
                    page.Window.window.pageStack.layers.pop();
                }
            }

            onClicked: deletePrompt.open()
        }
    }
}