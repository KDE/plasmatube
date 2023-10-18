// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2
import org.kde.kirigami 2 as Kirigami
import org.kde.purpose 1.0 as Purpose

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
                applicationWindow().pageStack.pushDialogLayer('qrc:/components/ShareDialog.qml', {
                    index: index,
                    model: alternativesModel
                })
            }
        }
    }
}