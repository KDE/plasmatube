// SPDX-FileCopyrightText: 2022 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.plasmatube 1.0

FormCard.FormCardPage {
    title: i18n("Settings")

    FormCard.FormHeader {
        title: i18n("Invidious")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            label: i18n("Instance")
            text: Settings !== null ? Settings.invidiousInstance : ""
            inputMethodHints: Qt.ImhUrlCharactersOnly
            onAccepted: Settings.invidiousInstance = text
        }
    }

    FormCard.FormHeader {
        title: i18n("Account")
    }

    FormCard.FormCard {
        QQC2.Label {
            id: isLoggedInLabel

            visible: PlasmaTube.isLoggedIn
            Layout.fillWidth: true
            text: i18n("Currently logged in as %1.", PlasmaTube.invidiousId)
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            padding: Kirigami.Units.largeSpacing
        }

        Loader {
            id: preferencesLoader

            active: PlasmaTube.isLoggedIn && PlasmaTube.preferences

            Layout.fillWidth: true

            sourceComponent: ColumnLayout {
                spacing: 0

                FormCard.FormDelegateSeparator {}

                FormCard.FormCheckDelegate {
                    text: i18n("Autoplay")
                    checked: PlasmaTube.preferences.autoPlay
                    onCheckedChanged: {
                        let preferences = PlasmaTube.preferences;
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
                            display: "Popular"
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
                        let defaultHome = PlasmaTube.preferences.defaultHome;
                        switch (defaultHome) {
                            case "Search":
                                currentIndex = 0;
                                break;
                            case "Popular":
                                currentIndex = 1;
                                break;
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
                        let preferences = PlasmaTube.preferences;
                        preferences.defaultHome = currentText;
                        PlasmaTube.preferences = preferences;
                    }
                }

                Connections {
                    target: PlasmaTube

                    function onPreferencesChanged() {
                        defaultHomepageDelegate.calculateIndex();
                    }
                }
            }
        }

        FormCard.FormDelegateSeparator {
            visible: PlasmaTube.isLoggedIn
            below: isLoggedInLabel
        }

        FormCard.FormButtonDelegate {
            visible: !PlasmaTube.isLoggedIn
            Layout.alignment: Qt.AlignHCenter
            text: i18n("Log in")
            onClicked: pageStack.layers.push("qrc:/LoginPage.qml");
        }

        FormCard.FormButtonDelegate {
            visible: PlasmaTube.isLoggedIn
            Layout.alignment: Qt.AlignHCenter
            text: i18n("Log out")
            onClicked: PlasmaTube.logOut();
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        FormCard.FormButtonDelegate {
            text: i18n("About PlasmaTube")
            onClicked: applicationWindow().pageStack.layers.push(aboutPage)

            Component {
                id: aboutPage

                FormCard.AboutPage {
                    aboutData: About
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: i18n("About KDE")
            onClicked: applicationWindow().pageStack.layers.push(aboutKDEPage)

            Component {
                id: aboutKDEPage

                FormCard.AboutKDE {}
            }
        }
    }
}
