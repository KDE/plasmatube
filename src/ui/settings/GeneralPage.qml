// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube.private

FormCard.FormCardPage {
    id: root

    title: i18n("General")

    FormCard.FormCard {
        Layout.fillWidth: true
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check Hide short-form videos", "Hide Shorts")
            description: i18n("Hide short-form videos designed for viewing on a mobile device.")
            checked: Settings.hideShorts
            enabled: !Settings.hideShortsImmutable
            onToggled: {
                Settings.hideShorts = checked;
                Settings.save();
            }
        }
    }
}
