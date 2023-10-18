// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiComponents

import org.kde.plasmatube 1.0

ColumnLayout {
    id: root

    function loadComments(vid) {
        commentsModel.fillComments(vid)
    }

    Repeater {
        anchors.fill: parent

        model: CommentsModel {
            id: commentsModel
        }

        delegate: RowLayout {
            required property string author
            required property string authorAvatar
            required property string content

            KirigamiComponents.Avatar {
                source: authorAvatar
                cache: true
                name: author
            }

            ColumnLayout {
                id: layout

                Layout.fillWidth: true
                Layout.bottomMargin: Kirigami.Units.smallSpacing

                spacing: 0
                clip: true

                Kirigami.Heading {
                    level: 4
                    text: author
                    type: Kirigami.Heading.Type.Primary
                    color: root.secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
                    verticalAlignment: Text.AlignTop
                    elide: Text.ElideRight
                    textFormat: Text.RichText
                    Layout.fillWidth: true
                }

                QQC2.Label {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    color: Kirigami.Theme.disabledTextColor
                    text: content
                    verticalAlignment: Text.AlignTop
                }
            }
        }
    }

    QQC2.Button {
        text: i18nc("@action:button Load more comments", "Load more")
        onClicked: commentsModel.loadMore()
    }
}