// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts

import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.plasmatube

BaseSourcePage {
    id: page

    FormCard.FormHeader {
        title: i18n("Account")
    }

    AccountCard {
        source: page.source
    }

    Loader {
        id: preferencesLoader

        active: page.source.loggedIn && page.source.preferences

        Layout.fillWidth: true

        sourceComponent: ColumnLayout {
            id: root

            property var selectedSource

            spacing: 0

            FormCard.FormHeader {
                title: i18n("Preferences")
            }

            FormCard.FormCard {
                FormCard.FormCheckDelegate {
                    text: i18n("Autoplay videos")
                    description: i18n("Whether to start playing videos immediately, or wait until the play button is pressed.")
                    checked: root.selectedSource.preferences.autoPlay
                    onToggled: {
                        let preferences = root.selectedSource.preferences;
                        preferences.autoPlay = checked;
                        PlasmaTube.preferences = preferences;
                    }
                }

                FormCard.FormDelegateSeparator {
                }

                FormCard.FormComboBoxDelegate {
                    id: defaultHomepageDelegate

                    text: i18n("Default Page")
                    description: i18n("The first page to load when switching to this source.")
                    textRole: "display"
                    valueRole: "value"
                    model: [
                        {
                            display: i18nc("@info Search page", "Search"),
                            value: "Search"
                        },
                        {
                            display: i18nc("@info Trending page", "Trending"),
                            value: "Trending"
                        },
                        {
                            display: i18nc("@info Subscriptions page", "Subscriptions"),
                            value: "Subscriptions"
                        },
                        {
                            display: i18nc("@info Playlists page", "Playlists"),
                            value: "Playlists"
                        },
                    ]

                    onActivated: {
                        let preferences = root.selectedSource.preferences;
                        preferences.defaultHome = currentValue;
                        PlasmaTube.preferences = preferences;
                    }
                }
            }

            onSelectedSourceChanged: defaultHomepageDelegate.currentIndex = defaultHomepageDelegate.indexOfValue(root.selectedSource.preferences.defaultHome)
        }

        onLoaded: item.selectedSource = page.source
    }
}