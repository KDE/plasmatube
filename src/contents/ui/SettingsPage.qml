// SPDX-FileCopyrightText: 2022 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.coreaddons as KCoreAddons

import org.kde.plasmatube
import org.kde.plasmatube.private
import org.kde.plasmatube.invidious

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
        title: i18n("Sources")
    }

    FormCard.FormCard {
        Layout.fillWidth: true
        Layout.topMargin: Kirigami.Units.largeSpacing

        Repeater {
            model: PlasmaTube.sourceManager

            ColumnLayout {
                required property var source
                required property int index

                spacing: 0

                FormCard.FormDelegateSeparator {
                    visible: index !== 0
                }

                FormCard.FormButtonDelegate {
                    text: source.url

                    description: {
                        switch (source.type) {
                            case VideoSource.Invidious:
                                return i18n("Invidious");
                            case VideoSource.PeerTube:
                                return i18n("PeerTube");
                        }
                    }

                    onClicked: {
                        let pageName;
                        switch (source.type) {
                            case VideoSource.Invidious:
                                pageName = "InvidiousSourcePage";
                                break;
                            case VideoSource.PeerTube:
                                pageName = "PeerTubeSourcePage";
                                break;
                        }

                        applicationWindow().pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", pageName), {
                            source
                        })
                    }
                }
            }
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
                    aboutData: KCoreAddons.AboutData
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
