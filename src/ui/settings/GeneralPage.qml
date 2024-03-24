// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.coreaddons as KCoreAddons

import org.kde.plasmatube
import org.kde.plasmatube.private
import org.kde.plasmatube.invidious

FormCard.FormCardPage {
    id: root

    title: i18n("General")

    FormCard.FormCard {
        Layout.fillWidth: true
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check Hide short-form videos", "Hide Shorts")
            checked: Settings.hideShorts
            enabled: !Settings.hideShortsImmutable
            onToggled: {
                Settings.hideShorts = checked;
                Settings.save();
            }
        }
    }
}
