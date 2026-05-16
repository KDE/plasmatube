// SPDX-FileCopyrightText: 2026 Plasmatube Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube

BaseSourcePage {
    id: page

    readonly property var browserOptions: [
        { name: "", label: i18n("None (anonymous)") },
        { name: "firefox", label: "Firefox" },
        { name: "chromium", label: "Chromium" },
        { name: "chrome", label: "Chrome" },
        { name: "brave", label: "Brave" },
        { name: "vivaldi", label: "Vivaldi" },
    ]

    FormCard.FormHeader {
        title: i18nc("@title:group", "yt-dlp")
    }

    FormCard.FormCard {
        FormCard.FormComboBoxDelegate {
            id: cookiesCombo

            text: i18n("Cookies from browser")
            description: i18n("Reuse YouTube cookies from a local browser to bypass anti-bot checks. Takes effect on next video load.")

            model: page.browserOptions.map(o => o.label)

            currentIndex: {
                const i = page.browserOptions.findIndex(o => o.name === page.source.cookiesFromBrowser);
                return i < 0 ? 0 : i;
            }

            onActivated: index => {
                page.source.cookiesFromBrowser = page.browserOptions[index].name;
            }
        }

        FormCard.FormDelegateSeparator {
            visible: profileField.visible
        }

        FormCard.FormTextFieldDelegate {
            id: profileField

            label: i18n("Browser profile")
            placeholderText: i18n("default")
            text: page.source.cookiesBrowserProfile
            visible: page.source.cookiesFromBrowser !== ""

            onEditingFinished: page.source.cookiesBrowserProfile = text
        }
    }
}
