/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
