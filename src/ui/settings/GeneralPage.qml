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
            id: hideShortsDelegater

            text: i18nc("@option:check Hide short-form videos", "Hide Shorts")
            description: i18n("Hide short-form videos designed for viewing on a mobile device.")
            checked: Settings.hideShorts
            enabled: !Settings.hideShortsImmutable
            onToggled: {
                Settings.hideShorts = checked;
                Settings.save();
            }
        }

        FormCard.FormDelegateSeparator {
            above: hideShortsDelegater
            below: checkSponsorsDelegate
        }

        FormCard.FormCheckDelegate {
            id: checkSponsorsDelegate

            text: i18nc("@option:check", "Check for sponsored segments")
            description: i18n("Inform you when a community-submitted segment (typically a sponsorship) is in the video. This feature requires contacting a 3rd-party service.")
            checked: Settings.useSponsorBlock
            enabled: !Settings.useSponsorBlockImmutable
            onToggled: {
                Settings.useSponsorBlock = checked;
                Settings.save();
            }
        }

        FormCard.FormDelegateSeparator {
            above: checkSponsorsDelegate
            below: skipSponsorsDelegate
        }

        FormCard.FormCheckDelegate {
            id: skipSponsorsDelegate

            text: i18nc("@option:check", "Skip sponsored segments")
            description: i18n("Automatically skips over sponsored segments in the video.")
            checked: Settings.skipSponsorBlock
            enabled: !Settings.skipSponsorBlockImmutable && Settings.useSponsorBlock
            onToggled: {
                Settings.skipSponsorBlock = checked;
                Settings.save();
            }
        }
    }
}
