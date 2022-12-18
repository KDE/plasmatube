// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.7 as Kirigami

import "utils.js" as Utils

Item {
    id: root
    property real leftPadding: Kirigami.Units.largeSpacing
    property real rightPadding: Kirigami.Units.largeSpacing
    property real topPadding: Kirigami.Units.largeSpacing
    property real bottomPadding: Kirigami.Units.largeSpacing

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

    signal clicked()

    ColumnLayout {
        id: gridLayout
        anchors.fill: parent
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding

        spacing: 0

        Image {
            id: thumb
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: gridLayout.width
            Layout.maximumWidth: gridLayout.width
            Layout.preferredHeight: gridLayout.width / 16 * 8
            Layout.maximumHeight: gridLayout.width / 16 * 8

            source: thumbnail
            fillMode: Image.PreserveAspectCrop

            Text {
                visible: !liveNow
                text: Utils.formatTime(length)
                color: "white"
                font.pointSize: Kirigami.Theme.smallFont.pointSize

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
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: root.clicked()
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
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.clicked()
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop

                Controls.Label {
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    text: author
                    color: Kirigami.Theme.disabledTextColor
                    maximumLineCount: 2
                    elide: Text.ElideRight

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
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    text: i18n("\u2022 %1 views \u2022 %2", Utils.formatCount(viewCount), liveNow ? "<i>live now</i>" : publishedText)
                    color: Kirigami.Theme.disabledTextColor
                    maximumLineCount: 1
                    elide: Text.ElideRight
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}

