// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import Qt5Compat.GraphicalEffects

import org.kde.kirigami as Kirigami

import org.kde.plasmatube

Kirigami.ApplicationWindow {
    id: root

    title: i18n("Picture in Picture")

    flags: Qt.Window | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint

    minimumWidth: 320
    minimumHeight: 240

    property alias renderer: renderer
    readonly property var video: PlasmaTube.videoController.currentVideo

    MouseArea {
        id: videoContainer
        anchors.fill: parent

        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
        Kirigami.Theme.inherit: false

        property bool showControls: false
        onClicked: {
            if (showControls) {
                controlTimer.stop();
                videoContainer.showControls = false;
            } else {
                showControls = true;
                controlTimer.restart();
            }
        }

        hoverEnabled: !Kirigami.Settings.tabletMode
        onContainsMouseChanged: {
            if (!pressed && hoverEnabled) {
                videoContainer.showControls = containsMouse;
            }
        }

        Timer {
            id: controlTimer
            interval: 2000
            onTriggered: videoContainer.showControls = false
        }

        MpvObject {
            id: renderer
            anchors.fill: parent

            visible: !stopped
        }
        Rectangle {
            anchors.fill: renderer

            color: "black"
            visible: renderer.stopped

            Image {
                id: thumbnailImage

                anchors.fill: parent
                source: video.thumbnailUrl("high")
            }

            QQC2.BusyIndicator {
                anchors.centerIn: parent
            }
        }

        VideoData {
            title: video.title
            visible: opacity > 0
            opacity: videoContainer.showControls ? 1 : 0
            Behavior on opacity {
                NumberAnimation {
                    duration: Kirigami.Units.veryLongDuration; easing.type: Easing.InOutCubic
                }
            }
        }

        VideoControls {
            inFullScreen: false
            anchors.fill: parent
            showPresentationControls: false

            visible: opacity > 0
            opacity: videoContainer.showControls ? 1 : 0
            Behavior on opacity {
                NumberAnimation {
                    duration: Kirigami.Units.veryLongDuration; easing.type: Easing.InOutCubic
                }
            }

            QQC2.ToolButton {
                anchors {
                    top: parent.top
                    right: parent.right
                }

                display: QQC2.AbstractButton.IconOnly

                icon.name: "window-close-symbolic"
                icon.color: "white"

                onClicked: root.close()
            }

            QQC2.ToolButton {
                id: resizeHandler

                anchors {
                    bottom: parent.bottom
                    right: parent.right
                }

                display: QQC2.AbstractButton.IconOnly

                icon.name: "drag-surface"

                hoverEnabled: true
            }
        }
    }

    DragHandler {
        grabPermissions:  PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType | PointerHandler.ApprovesTakeOverByAnything
        onActiveChanged: {
            if (active) {
                if (resizeHandler.hovered) {
                    root.startSystemResize(Qt.RightEdge | Qt.BottomEdge);
                } else {
                    root.startSystemMove();
                }
            }
        }
        target: null
    }
}