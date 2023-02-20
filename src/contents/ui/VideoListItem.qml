// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.7 as Kirigami
import QtGraphicalEffects 1.0


import "utils.js" as Utils

MouseArea {
    id: root
    implicitWidth: gridLayout.implicitWidth + leftPadding + rightPadding
    implicitHeight: gridLayout.implicitHeight + topPadding + bottomPadding

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

    property real zoomScale: (root.pressed || thumbnailMouseArea.pressed || titleMouseArea.pressed) ? 0.9 : 1
    Behavior on zoomScale {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutExpo
        }
    }

    transform: Scale {
        origin.x: root.width / 2;
        origin.y: root.height / 2;
        xScale: root.zoomScale
        yScale: root.zoomScale
    }

    RowLayout {
        id: gridLayout
        anchors.top: parent.top
        anchors.topMargin: root.topPadding
        anchors.left: parent.left
        anchors.leftMargin: root.leftPadding
        anchors.right: parent.right
        anchors.rightMargin: root.rightPadding

        spacing: Kirigami.Units.largeSpacing

        Image {
            id: thumb
            Layout.preferredWidth: root.width < 500 ? Kirigami.Units.gridUnit * 8 : Kirigami.Units.gridUnit * 12
            Layout.preferredHeight: root.width < 500 ? Kirigami.Units.gridUnit * 4.5 : Kirigami.Units.gridUnit * 6.75
            Layout.maximumWidth: root.width < 500 ? Kirigami.Units.gridUnit * 8 : Kirigami.Units.gridUnit * 12
            Layout.maximumHeight: root.width < 500 ? Kirigami.Units.gridUnit * 4.5 : Kirigami.Units.gridUnit * 6.75
            source: thumbnail
            fillMode: Image.PreserveAspectCrop
            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: mask
            }
            Rectangle {
                id: mask
                radius: 7
                anchors.fill: thumb
                visible: false
            }
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

            MouseArea {
                id: thumbnailMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: root.clicked(mouse)
            }
        }

        ColumnLayout {
            id: videoInfo
            Layout.alignment: Qt.AlignTop
            // Layout.preferredHeight: thumb.height
            spacing: Kirigami.Units.smallSpacing

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

            Flow {
                Layout.fillWidth: true

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
                    text: i18n(" \u2022 %1 views", Utils.formatCount(viewCount))
                    color: Kirigami.Theme.disabledTextColor
                    maximumLineCount: 1
                    elide: Text.ElideRight
                }

                Controls.Label {
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    text: i18n(" \u2022 %1", liveNow ? "<i>live now</i>" : publishedText)
                    color: Kirigami.Theme.disabledTextColor
                    maximumLineCount: 1
                    elide: Text.ElideRight
                }
            }
        }
    }
}
