// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Window
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.plasmatube

BaseSourcePage {
    id: page

    FormCard.FormHeader {
        title: i18n("Account")
    }

    FormCard.FormCard {
        QQC2.Label {
            id: isLoggedInLabel

            visible: page.source.loggedIn
            Layout.fillWidth: true
            text: i18n("Currently logged in as %1.", page.source.username)
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            padding: Kirigami.Units.largeSpacing
        }

        FormCard.FormDelegateSeparator {
            above: isLoggedInLabel
            below: loginButton
            visible: page.source.loggedIn
        }

        FormCard.FormButtonDelegate {
            id: loginButton

            text: {
                if (page.source.loggedIn) {
                    return i18nc("@action:button", "Log out")
                } else {
                    return i18nc("@action:button", "Log in")
                }
            }
            icon.name: page.source.loggedIn ? "go-previous-symbolic" : "im-user"
            description: !page.source.loggedIn ? i18n("Subscribe to channels, keep track of watched videos and more.") : ""
            onClicked: {
                if (page.source.loggedIn) {
                    page.source.logOut();
                } else {
                    page.Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "LoginPage"), {source: page.source});
                }
            }
        }
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
                    Layout.fillWidth: true
                    text: i18n("Default homepage")
                    description: i18n("The default page to load when switching to this source.")
                    textRole: "display"
                    valueRole: "display"
                    // TODO: these need to be localized, but ListElement makes this difficult...
                    model: ListModel {
                        ListElement {
                            display: "Search"
                        }
                        ListElement {
                            display: "Trending"
                        }
                        ListElement {
                            display: "Subscriptions"
                        }
                        ListElement {
                            display: "Playlists"
                        }
                    }

                    function calculateIndex() {
                        let defaultHome = root.selectedSource.preferences.defaultHome;
                        switch (defaultHome) {
                            case "Search":
                                currentIndex = 0;
                            case "Trending":
                                currentIndex = 1;
                                break;
                            case "Subscriptions":
                                currentIndex = 2;
                                break;
                            case "Playlists":
                                currentIndex = 3;
                                break;
                        }
                    }

                    onCurrentValueChanged: {
                        let preferences = root.selectedSource.preferences;
                        preferences.defaultHome = currentText;
                        PlasmaTube.preferences = preferences;
                    }
                }
            }

            onSelectedSourceChanged: defaultHomepageDelegate.calculateIndex()
        }

        onLoaded: item.selectedSource = page.source
    }
}