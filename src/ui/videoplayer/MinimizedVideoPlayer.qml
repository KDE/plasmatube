// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Effects

import org.kde.kirigami as Kirigami
import org.kde.plasmatube

Rectangle {
    id: root
    property alias previewSource: blur.source

    property string videoName
    property string channelName

    readonly property bool isPlaying: PlasmaTube.videoController.currentPlayer ? !PlasmaTube.videoController.currentPlayer.paused : false

    signal toggleRequested()
    signal stopRequested()
    signal openRequested()
    signal nextRequested()

    color: Kirigami.Theme.backgroundColor

    Kirigami.Theme.colorSet: Kirigami.Theme.Header
    Kirigami.Theme.inherit: false

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true

            // press feedback
            color: (trackClick.pressed || trackClick.containsMouse) ? Qt.rgba(0, 0, 0, 0.1) : "transparent"

            RowLayout {
                spacing: Kirigami.Units.largeSpacing
                anchors.fill: parent

                // video preview
                MultiEffect {
                    id: blur
                    Layout.fillHeight: true
                    implicitWidth: Kirigami.Units.gridUnit * 3
                }

                ColumnLayout {
                    spacing: Kirigami.Units.smallSpacing
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    QQC2.Label {
                        Layout.fillWidth: true
                        text: root.videoName
                        elide: Text.ElideRight
                    }

                    QQC2.Label {
                        Layout.fillWidth: true
                        font: Kirigami.Theme.smallFont
                        text: root.channelName
                        elide: Text.ElideRight
                        opacity: 0.9
                    }
                }
            }

            MouseArea {
                id: trackClick
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.openRequested()
            }
        }

        QQC2.Button {
            flat: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
            icon.name: root.isPlaying ? "media-playback-pause" : "media-playback-start"
            icon.height: Kirigami.Units.iconSizes.small
            icon.width: Kirigami.Units.iconSizes.small
            onClicked: root.toggleRequested()
        }

        QQC2.Button {
            flat: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
            icon.name: "media-playback-stop"
            icon.height: Kirigami.Units.iconSizes.small
            icon.width: Kirigami.Units.iconSizes.small
            onClicked: root.stopRequested()
        }

        QQC2.Button {
            flat: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
            icon.name: "media-skip-forward"
            icon.height: Kirigami.Units.iconSizes.small
            icon.width: Kirigami.Units.iconSizes.small
            onClicked: root.nextRequested()
        }
    }
}
