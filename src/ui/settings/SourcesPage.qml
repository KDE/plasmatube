// SPDX-FileCopyrightText: 2022 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube
import org.kde.plasmatube.private

FormCard.FormCardPage {
    id: root

    title: i18n("Sources")

    actions: [
        Kirigami.Action {
            text: i18nc("@action:button", "Add Source…")
            icon.name: "list-add"
            onTriggered: root.Window.window.pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "WelcomePage"), {}, {title: i18nc("@title:window", "Welcome")})
        }
    ]

    FormCard.FormCard {
        Layout.fillWidth: true
        Layout.topMargin: Kirigami.Units.largeSpacing

        Repeater {
            model: PlasmaTube.sourceManager

            ColumnLayout {
                id: sourceDelegate

                required property var source
                required property int index

                spacing: 0

                FormCard.FormDelegateSeparator {
                    visible: sourceDelegate.index !== 0
                }

                FormCard.FormButtonDelegate {
                    text: sourceDelegate.source.url

                    icon.name: {
                        switch (sourceDelegate.source.type) {
                            case VideoSource.Invidious:
                                return "plasmatube-invidious";
                            case VideoSource.PeerTube:
                                return "plasmatube-peertube";
                            case VideoSource.Piped:
                                return "plasmatube-piped";
                        }
                    }

                    description: {
                        switch (sourceDelegate.source.type) {
                            case VideoSource.Invidious:
                                return i18n("Invidious");
                            case VideoSource.PeerTube:
                                return i18n("PeerTube");
                            case VideoSource.Piped:
                                return i18n("Piped");
                        }
                    }

                    onClicked: {
                        let pageName;
                        switch (sourceDelegate.source.type) {
                            case VideoSource.Invidious:
                                pageName = "InvidiousSourcePage";
                                break;
                            case VideoSource.PeerTube:
                                pageName = "PeerTubeSourcePage";
                                break;
                            case VideoSource.Piped:
                                pageName = "PipedSourcePage";
                                break;
                        }

                        root.Window.window.pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", pageName), {
                            source
                        })
                    }
                }
            }
        }
    }
}
