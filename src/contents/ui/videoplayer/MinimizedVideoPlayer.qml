// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.14
import QtMultimedia 5.15

import QtGraphicalEffects 1.15

import org.kde.kirigami 2.12 as Kirigami

Rectangle {
    id: root
    property alias previewSource: blur.source

    property string videoName
    property string channelName

    property bool isPlaying

    signal toggleRequested()
    signal stopRequested()
    signal openRequested()

    color: Kirigami.Theme.backgroundColor

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true

            // press feedback
            color: (trackClick.pressed || trackClick.containsMouse) ? Qt.rgba(0, 0, 0, 0.05) : "transparent"

            RowLayout {
                spacing: Kirigami.Units.largeSpacing
                anchors.fill: parent

                // video preview
                FastBlur {
                    id: blur
                    radius: 0
                    Layout.fillHeight: true
                    implicitWidth: Kirigami.Units.gridUnit * 3
                }

                ColumnLayout {
                    spacing: Kirigami.Units.smallSpacing
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    Controls.Label {
                        Layout.fillWidth: true
                        text: root.videoName
                        elide: Text.ElideRight
                    }

                    Controls.Label {
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

        Controls.Button {
            flat: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
            icon.name: root.isPlaying ? "media-playback-pause" : "media-playback-start"
            icon.height: Kirigami.Units.iconSizes.smallMedium
            icon.width: Kirigami.Units.iconSizes.smallMedium
            onClicked: root.toggleRequested()
        }

        Controls.Button {
            flat: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
            icon.name: "media-playback-stop"
            icon.height: Kirigami.Units.iconSizes.smallMedium
            icon.width: Kirigami.Units.iconSizes.smallMedium
            onClicked: root.stopRequested()
        }
    }
}
