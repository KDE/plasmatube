// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.purpose as Purpose

QQC2.Menu {
    id: root

    required property string url
    required property string shareTitle

    title: i18n("Share")
    icon.name: "emblem-shared-symbolic"

    Repeater {
        model: Purpose.PurposeAlternativesModel {
            id: alternativesModel
            inputData: {
                'urls': [root.url],
                'title': root.shareTitle,
                'mimeType': "*"
            }
            pluginType: "ShareUrl"
        }

        delegate: QQC2.MenuItem {
            text: model.display
            icon.name: model.iconName

            onTriggered: {
                applicationWindow().pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "ShareDialog"), {
                    index: index,
                    model: alternativesModel
                })
            }
        }
    }
}