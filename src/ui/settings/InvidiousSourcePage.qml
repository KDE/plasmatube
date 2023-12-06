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
        }

        Loader {
            id: preferencesLoader

            active: page.source.loggedIn && page.source.preferences

            Layout.fillWidth: true

            sourceComponent: ColumnLayout {
                id: root

                property var selectedSource

                spacing: 0

                FormCard.FormCheckDelegate {
                    text: i18n("Autoplay")
                    checked: root.selectedSource.preferences.autoPlay
                    onToggled: {
                        let preferences = root.selectedSource.preferences;
                        preferences.autoPlay = checked;
                        PlasmaTube.preferences = preferences;
                    }
                }

                FormCard.FormDelegateSeparator {}

                FormCard.FormComboBoxDelegate {
                    id: defaultHomepageDelegate
                    Layout.fillWidth: true
                    text: i18n("Default homepage")
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
                                break;
                            // Popular would take index 1
                            case "Trending":
                                currentIndex = 2;
                                break;
                            case "Subscriptions":
                                currentIndex = 3;
                                break;
                            case "Playlists":
                                currentIndex = 4;
                                break;
                        }
                    }

                    onCurrentValueChanged: {
                        let preferences = root.selectedSource.preferences;
                        preferences.defaultHome = currentText;
                        PlasmaTube.preferences = preferences;
                    }
                }

                FormCard.FormDelegateSeparator {}

                onSelectedSourceChanged: defaultHomepageDelegate.calculateIndex()
            }

            onLoaded: item.selectedSource = page.source
        }

        FormCard.FormButtonDelegate {
            visible: !page.source.loggedIn
            Layout.alignment: Qt.AlignHCenter
            text: i18n("Log in")
            onClicked: page.Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "LoginPage"), {source: page.source});
        }

        FormCard.FormButtonDelegate {
            visible: page.source.loggedIn
            Layout.alignment: Qt.AlignHCenter
            text: i18n("Log out")
            onClicked: page.source.logOut();
        }
    }

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