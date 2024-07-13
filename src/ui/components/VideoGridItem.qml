// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.coreaddons as KCoreAddons

import "utils.js" as Utils

BaseGridItem {
    id: root

    property string vid
    property url thumbnail
    property bool liveNow
    property int length
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

        spacing: Kirigami.Units.largeSpacing

        TapHandler {
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            acceptedButtons: Qt.RightButton
            onTapped: root.contextMenuRequested()
        }

        TapHandler {
            acceptedDevices: PointerDevice.TouchScreen
            onLongPressed: root.contextMenuRequested()
        }

        Image {
            id: thumb
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: column.width
            Layout.maximumWidth: column.width
            Layout.preferredHeight: column.width / 16 * 9
            Layout.maximumHeight: column.width / 16 * 9
            layer.enabled: true
            layer.effect: RoundedEffect {}
            source: root.thumbnail
            fillMode: Image.PreserveAspectCrop

            PlaceholderItem {
                anchors.fill: parent
                visible: thumb.status !== Image.Ready
            }

            Text {
                text: {
                    if (root.liveNow) {
                        return i18nc("Short label for a livestream that's live", "Live");
                    }

                    const time = KCoreAddons.Format.formatDuration(length * 1000);
                    if (time !== "00:00") {
                        return time;
                    } else {
                        return i18nc("Short label for a short video", "Short");
                    }
                }
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
            spacing: Kirigami.Units.smallSpacing

            Kirigami.Heading {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true

                text: root.title
                level: 4
                maximumLineCount: 2
                wrapMode: Text.Wrap
                elide: Text.ElideRight
            }

            QQC2.Label {
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                text: root.author
                color: Kirigami.Theme.disabledTextColor
                maximumLineCount: 2
                elide: Text.ElideRight
            }

            QQC2.Label {
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                text: {
                    let views;
                    if (root.viewCount === 0) {
                        views = i18n("No views");
                    } else {
                        views = i18n("%1 views", Utils.formatCount(root.viewCount));
                    }

                    let date = liveNow ? i18n("<i>live now</i>") : root.publishedText;
                    return views + " \u2022 " + date;
                }
                visible: root.liveNow || root.viewCount !== 0
                color: Kirigami.Theme.disabledTextColor
                maximumLineCount: 1
                elide: Text.ElideRight
            }
        }

        Item { Layout.fillHeight: true }
    }
}

