// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

import org.kde.kirigami as Kirigami

BaseGridItem {
    id: root

    property string title
    property string thumbnail
    property int videoCount

    contentItem: ColumnLayout {
        id: column
        anchors.fill: parent
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding

        spacing: 0

        Image {
            id: thumb
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: column.width
            Layout.maximumWidth: column.width
            Layout.preferredHeight: column.width / 16 * 9
            Layout.maximumHeight: column.width / 16 * 9
            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: mask
            }
            source: thumbnail
            fillMode: Image.PreserveAspectCrop
            Rectangle {
                id: mask
                radius: 7
                anchors.fill: thumb
                visible: false
            }

            PlaceholderItem {
                anchors.fill: parent
                visible: thumb.status !== Image.Ready
            }

            Text {
                text: i18np("%1 video", "%1 videos", videoCount)
                color: "white"
                font.pointSize: Kirigami.Theme.smallFont.pointSize

                z: 2

                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: Kirigami.Units.smallSpacing * 2
                anchors.bottomMargin: Kirigami.Units.smallSpacing * 2

                Rectangle {
                    anchors.fill: parent
                    anchors.topMargin: -Kirigami.Units.smallSpacing
                    anchors.bottomMargin: -Kirigami.Units.smallSpacing
                    anchors.leftMargin: -Kirigami.Units.smallSpacing
                    anchors.rightMargin: -Kirigami.Units.smallSpacing
                    z: -1
                    color: "#90000000"
                    radius: 2
                    width: 60
                    height: 15
                }
            }

            MouseArea {
                id: thumbnailMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: (event) => root.clicked(event)
            }
        }

        ColumnLayout {
            id: videoInfo
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
            spacing: Kirigami.Units.largeSpacing

            Kirigami.Heading {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true

                text: title
                level: 4
                maximumLineCount: 2
                wrapMode: Text.Wrap
                elide: Text.ElideRight

                MouseArea {
                    id: titleMouseArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.clicked(mouse)
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}

