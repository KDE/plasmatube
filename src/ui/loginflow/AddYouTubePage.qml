// SPDX-FileCopyrightText: 2026 Plasmatube Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube

FormCard.FormCardPage {
    id: root

    title: i18nc("@title:window", "Add YouTube Source")

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormTextDelegate {
            text: i18n("Talks to YouTube directly via the Innertube API and resolves video streams with yt-dlp.")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: i18nc("@action:button", "Add Source")
            icon.name: "list-add"
            onClicked: {
                PlasmaTube.sourceManager.createYouTubeSource();
                if (root.Window.window !== applicationWindow()) {
                    root.Window.window.close();
                }
            }
        }
    }
}
