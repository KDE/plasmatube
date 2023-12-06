// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import Qt5Compat.GraphicalEffects

import org.kde.kirigami as Kirigami

import "utils.js" as Utils

QQC2.ItemDelegate {
    id: root

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
    property bool watched

    signal contextMenuRequested

    leftPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing

    hoverEnabled: true
    onPressAndHold: contextMenuRequested()

    property real zoomScale: pressed ? 0.9 : 1
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

    background: Rectangle {
        anchors.fill: parent
        radius: Kirigami.Units.smallSpacing
        visible: root.hovered || root.activeFocus || root.highlighted
        color: Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.hoverColor, 0.2);
        border.color: Kirigami.Theme.hoverColor
        border.width: 1
    }

    contentItem: RowLayout {
        id: gridLayout
        anchors.top: parent.top
        anchors.topMargin: root.topPadding
        anchors.left: parent.left
        anchors.leftMargin: root.leftPadding
        anchors.right: parent.right
        anchors.rightMargin: root.rightPadding

        spacing: Kirigami.Units.largeSpacing

        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: root.contextMenuRequested()
        }

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
                visible: !liveNow && text !== "00:00"
                text: Utils.formatTime(length)
                color: "white"

                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: 7
                anchors.bottomMargin: 3

                z: 2

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

            Rectangle {
                id: watchIndicator

                color: "black"
                opacity: 0.5
                visible: root.watched
                anchors.fill: parent

                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }

                    color: "red"
                    height: 3
                }
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
            }

            Flow {
                Layout.fillWidth: true

                QQC2.Label {
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    text: author
                    color: Kirigami.Theme.disabledTextColor
                    maximumLineCount: 2
                    elide: Text.ElideRight
                }

                QQC2.Label {
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    text: i18n(" \u2022 %1 views", Utils.formatCount(viewCount))
                    color: Kirigami.Theme.disabledTextColor
                    maximumLineCount: 1
                    elide: Text.ElideRight
                }

                QQC2.Label {
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    text: i18n(" \u2022 %1", liveNow ? "<i>live now</i>" : publishedText)
                    color: Kirigami.Theme.disabledTextColor
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    visible: publishedText.length !== 0
                }
            }
        }
    }
}
