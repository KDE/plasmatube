// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.7 as Kirigami

import "utils.js" as Utils

Kirigami.AbstractListItem {
    property string vid
    property url thumbnail
    property bool liveNow
    property date length
    property string title
    property string author
    property string authorId
    property string description
    property int viewCount
    property string publishedText

    RowLayout {
        Image {
            id: thumb
            Layout.fillHeight: true
            Layout.preferredWidth: 208
            Layout.preferredHeight: 111
            Layout.maximumWidth: root.width * 0.3
            Layout.maximumHeight: root.width * 0.3 / 16.0 * 9.0
            source: thumbnail
            fillMode: Image.PreserveAspectFit

            Text {
                visible: !liveNow
                text: Utils.formatTime(length)
                color: "white"

                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: 7
                anchors.bottomMargin: 3

                Rectangle {
                    anchors.fill: parent
                    anchors.leftMargin: -2
                    anchors.rightMargin: -2
                    z: -1
                    color: "#90000000"
                    radius: 2
                    width: 60
                    height: 15
                }
            }
        }

        ColumnLayout {
            id: videoInfo
            Layout.preferredHeight: thumb.height
            Layout.maximumHeight: root.width * 0.3 / 16.0 * 9.0
            Layout.minimumHeight: 70
            spacing: 0

            Kirigami.Heading {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true

                text: title
                level: 4
                maximumLineCount: 1
                elide: Text.ElideRight
            }
            Controls.Label {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true

                text: author
                font.pointSize: 9
                font.italic: true
                maximumLineCount: 1
                elide: Text.ElideRight
                color: mouseArea.containsMouse ? Kirigami.Theme.highlightColor: Kirigami.Theme.textColor
                MouseArea {
                    id: mouseArea
                    cursorShape: Qt.PointingHandCursor
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        pageStack.push("qrc:/ChannelPage.qml", {author, authorId})
                    }
                }
            }
            Controls.Label {
                visible: root.width > 580
                enabled: root.width > 580
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true

                text: description
                font.pointSize: 8
                maximumLineCount: 2
                wrapMode: Text.Wrap
                elide: Text.ElideRight
            }
            Controls.Label {
                Layout.alignment: Qt.AlignBottom
                text: Utils.formatCount(viewCount) + " views \u2022 " +
                      (liveNow ? "<i>live now</i>" : publishedText)
                font.pointSize: 9
            }
        }
    }
}
