// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube

FormCard.FormCardPage {
    id: root

    title: i18n("General")

    FormCard.FormCard {
        Layout.fillWidth: true
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormComboBoxDelegate {
            id: preferredVideoQualityDelegate
            text: i18n("Preferred Video Quality")
            description: i18n("This selected quality will be used if available, otherwise the next highest one is used.")
            textRole: "display"
            valueRole: "value"
            currentIndex: PlasmaTube.settings.preferredVideoQuality
            model: [
                {
                    display: i18nc("@info video resolution", "2160p"),
                    value: Settings.Q2160
                },
                {
                    display: i18nc("@info video resolution", "1440p"),
                    value: Settings.Q1440
                },
                {
                    display: i18nc("@info video resolution", "1080p"),
                    value: Settings.Q1080
                },
                {
                    display: i18nc("@info video resolution", "720p"),
                    value: Settings.Q720
                },
                {
                    display: i18nc("@info video resolution", "480p"),
                    value: Settings.Q480
                },
                {
                    display: i18nc("@info video resolution", "360p"),
                    value: Settings.Q360
                },
                {
                    display: i18nc("@info video resolution", "240p"),
                    value: Settings.Q240
                },
                {
                    display: i18nc("@info video resolution", "144p"),
                    value: Settings.Q144
                }
            ]
            onActivated: {
                PlasmaTube.settings.preferredVideoQuality = currentValue;
                PlasmaTube.settings.save();
            }
        }

        FormCard.FormDelegateSeparator {
            above: preferredVideoQualityDelegate
            below: hideShortsDelegater
        }

        FormCard.FormCheckDelegate {
            id: hideShortsDelegater

            text: i18nc("@option:check Hide short-form videos", "Hide Shorts")
            description: i18n("Hide short-form videos designed for viewing on a mobile device.")
            checked: PlasmaTube.settings.hideShorts
            enabled: !PlasmaTube.settings.hideShortsImmutable
            onToggled: {
                PlasmaTube.settings.hideShorts = checked;
                PlasmaTube.settings.save();
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
            checked: PlasmaTube.settings.useSponsorBlock
            enabled: !PlasmaTube.settings.useSponsorBlockImmutable
            onToggled: {
                PlasmaTube.settings.useSponsorBlock = checked;
                PlasmaTube.settings.save();
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
            checked: PlasmaTube.settings.skipSponsorBlock
            enabled: !PlasmaTube.settings.skipSponsorBlockImmutable && PlasmaTube.settings.useSponsorBlock
            onToggled: {
                PlasmaTube.settings.skipSponsorBlock = checked;
                PlasmaTube.settings.save();
            }
        }
    }
}
