// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiComponents

import org.kde.plasmatube

ColumnLayout {
    id: root

    spacing: Kirigami.Units.mediumSpacing

    function loadComments(vid) {
        commentsModel.fillComments(vid)
    }

    Repeater {
        id: commentsRepeater

        Layout.fillWidth: true
        Layout.fillHeight: true

        model: CommentsModel {
            id: commentsModel
        }

        delegate: RowLayout {
            id: delegate

            width: commentsRepeater.width

            required property string author
            required property string authorAvatar
            required property string content

            KirigamiComponents.Avatar {
                source: delegate.authorAvatar
                cache: true
                name: delegate.author

                Layout.alignment: Qt.AlignTop
            }

            ColumnLayout {
                id: layout

                Layout.fillWidth: true

                spacing: Kirigami.Units.mediumSpacing
                clip: true

                Kirigami.Heading {
                    level: 4
                    text: delegate.author
                    type: Kirigami.Heading.Type.Primary
                    color: Kirigami.Theme.textColor
                    verticalAlignment: Text.AlignTop
                    elide: Text.ElideRight
                    textFormat: Text.RichText

                    Layout.fillWidth: true
                }

                QQC2.Label {
                    color: Kirigami.Theme.disabledTextColor
                    text: delegate.content
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.WordWrap
                    textFormat: Text.RichText

                    onHoveredLinkChanged: if (hoveredLink.length > 0 && hoveredLink !== "1") {
                        applicationWindow().hoverLinkIndicator.text = hoveredLink;
                    } else {
                        applicationWindow().hoverLinkIndicator.text = "";
                    }

                    onLinkActivated: (link) => Qt.openUrlExternally(link)

                    HoverHandler {
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.IBeamCursor
                    }

                    Layout.fillWidth: true
                }
            }
        }
    }

    QQC2.Button {
        text: i18nc("@action:button Load more comments", "Load more")
        onClicked: commentsModel.loadMore()
    }
}