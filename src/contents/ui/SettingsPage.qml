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

                MobileForm.FormTextDelegate {
                    visible: PlasmaTube.isLoggedIn
                    text: i18n("Currently logged in as:")
                    description: PlasmaTube.invidiousId
                }

                MobileForm.FormDelegateSeparator {
                    above: logout
                    visible: PlasmaTube.isLoggedIn
                }

                MobileForm.FormButtonDelegate {
                    id: logout
                    visible: PlasmaTube.isLoggedIn
                    Layout.alignment: Qt.AlignHCenter
                    text: i18n("Log out")
                    onClicked: PlasmaTube.logOut();
                }

                MobileForm.FormSectionText {
                    visible: !PlasmaTube.isLoggedIn
                    text: i18n("Please visit the website to register with an invidious instance.\nThere is currently no API for registering.")
                }

                MobileForm.AbstractFormDelegate {
                    Layout.fillWidth: true

                    visible: !PlasmaTube.isLoggedIn
                    background: Item {}

                    contentItem: ColumnLayout {
                        QQC2.Label {
                            text: i18n("Instance")
                            Layout.fillWidth: true
                        }

                        QQC2.ComboBox {
                            id: invidiousInstance

                            editable: true
                            model: ["https://ytprivate.com", "https://invidiou.site"]
                            Layout.fillWidth: true
                        }
                    }
                }

                MobileForm.FormDelegateSeparator {
                    visible: !PlasmaTube.isLoggedIn
                }

                MobileForm.FormTextFieldDelegate {
                    id: usernameField

                    label: i18n("Username")
                    visible: !PlasmaTube.isLoggedIn
                    onAccepted: passwordField.forceActiveFocus()
                }

                MobileForm.FormDelegateSeparator {
                    visible: !PlasmaTube.isLoggedIn
                }

                MobileForm.AbstractFormDelegate {
                    Layout.fillWidth: true

                    visible: !PlasmaTube.isLoggedIn

                    background: Item {}

                    contentItem: ColumnLayout {
                        QQC2.Label {
                            text: i18n("Password")
                            Layout.fillWidth: true
                        }

                        Kirigami.PasswordField {
                            id: passwordField

                            onAccepted: logInButton.clicked()

                            Layout.fillWidth: true
                        }
                    }
                }

                MobileForm.FormDelegateSeparator {
                    above: logInButton
                    visible: !PlasmaTube.isLoggedIn
                }

                MobileForm.FormButtonDelegate {
                    id: logInButton
                    visible: !PlasmaTube.isLoggedIn
                    text: i18n("Sign in")
                    onClicked: logInController.logIn(usernameField.text, passwordField.text, invidiousInstance.editText)
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
