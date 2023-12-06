// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import Qt5Compat.GraphicalEffects

import org.kde.kirigami as Kirigami

import "utils.js" as Utils

BaseGridItem {
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

    contentItem: ColumnLayout {
        id: column
        anchors.fill: parent
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding

        spacing: 0

        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: root.contextMenuRequested()
        }

        Image {
            id: thumb
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: column.width
            Layout.maximumWidth: column.width
            Layout.preferredHeight: column.width / 16 * 8
            Layout.maximumHeight: column.width / 16 * 8
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

            Text {
                visible: !liveNow && text !== "00:00"
                text: Utils.formatTime(length)
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
            }

            Flow {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop

                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    text: author
                    color: Kirigami.Theme.disabledTextColor
                    maximumLineCount: 2
                    elide: Text.ElideRight
                }

                QQC2.Label {
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

