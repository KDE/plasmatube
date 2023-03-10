// SPDX-FileCopyrightText: 2022 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

import org.kde.plasmatube 1.0

Kirigami.ScrollablePage {
    title: i18n("Settings")

    ColumnLayout {
        spacing: 0

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0
                MobileForm.FormCardHeader {
                    title: i18n("Invidious")
                }

                MobileForm.FormTextFieldDelegate {
                    label: i18n("Instance")
                    text: Settings !== null ? Settings.invidiousInstance : ""
                    inputMethodHints: Qt.ImhUrlCharactersOnly
                    onAccepted: Settings.invidiousInstance = text
                }
            }
        }

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Account")
                }

                QQC2.Label {
                    visible: PlasmaTube.isLoggedIn
                    Layout.fillWidth: true
                    text: i18n("Currently logged in as %1.", PlasmaTube.invidiousId)
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    padding: Kirigami.Units.largeSpacing
                }

                MobileForm.FormDelegateSeparator {
                    visible: PlasmaTube.isLoggedIn
                }

                MobileForm.FormButtonDelegate {
                    visible: !PlasmaTube.isLoggedIn
                    Layout.alignment: Qt.AlignHCenter
                    text: i18n("Log in")
                    onClicked: pageStack.layers.push("qrc:/LoginPage.qml");
                }

                MobileForm.FormButtonDelegate {
                    visible: PlasmaTube.isLoggedIn
                    Layout.alignment: Qt.AlignHCenter
                    text: i18n("Log out")
                    onClicked: PlasmaTube.logOut();
                }
            }
        }

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0
                Component {
                    id: aboutPage
                    MobileForm.AboutPage {
                        aboutData: About
                    }
                }
                MobileForm.FormButtonDelegate {
                    text: i18n("About PlasmaTube")
                    onClicked: applicationWindow().pageStack.layers.push(aboutPage)
                }
            }
        }
    }
}
