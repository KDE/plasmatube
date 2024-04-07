// SPDX-FileCopyrightText: 2020-2022 Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2021 Bart De Vries <bart@mogwai.be>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.plasmatube

/*
 * Holds the video player page together. It's twice the size of the window and just moves the page up and down.
 */
Flickable {
    id: root
    boundsBehavior: Flickable.StopAtBounds
    contentHeight: applicationWindow().height * 2
    interactive: Kirigami.Settings.hasTransientTouchInput

    property bool maximized: false
    property real contentToPlayerSpacing: 0

    property alias previewSource: videoPlayer.previewSource

    readonly property bool isVideoLoaded: PlasmaTube.videoController.currentVideo.isLoaded
    readonly property real miniPlayerHeight: Kirigami.Units.gridUnit * 3 + Kirigami.Units.gridUnit / 6

    function open() {
        toOpen.restart();
        maximized = true;
    }

    function close() {
        toClose.restart();
        maximized = false;
    }

    function resetToBoundsOnFlick() {
        if (!atYBeginning || !atYEnd) {
            if (root.verticalVelocity > 0) {
                toOpen.restart();
            } else if (root.verticalVelocity < 0) {
                toClose.restart();
            } else { // i.e. when verticalVelocity === 0
                if (contentY > contentHeight / 4) {
                    toOpen.restart();
                } else  {
                    toClose.restart();
                }
            }
        }
    }

    function resetToBoundsOnResize() {
        if (maximized) {
            contentY = contentHeight / 2;
        } else {
            contentY = 0;
        }
    }

    NumberAnimation on contentY {
        id: toOpen
        from: root.contentY
        to: root.contentHeight / 2
        duration: Kirigami.Units.longDuration * 2
        easing.type: Easing.OutCubic
        running: false
        onFinished: videoPlayer.forceActiveFocus(Qt.PopupFocusReason)
    }
    NumberAnimation on contentY {
        id: toClose
        from: root.contentY
        to: 0
        duration: Kirigami.Units.longDuration * 2
        easing.type: Easing.OutCubic
        running: false
    }

    // snap to end
    MouseArea {
        anchors.fill: contentZone
        propagateComposedEvents: true
        onPressed: {
            toOpen.stop();
            toClose.stop();
            propagateComposedEvents = true;
        }
        onReleased: root.resetToBoundsOnFlick()
    }

    onMovementStarted: {
        toOpen.stop();
        toClose.stop();
    }
    onFlickStarted: resetToBoundsOnFlick()
    onMovementEnded: resetToBoundsOnFlick()

    Connections {
        target: applicationWindow()

        function onHeightChanged() {
            root.resetToBoundsOnResize();
        }
    }

    ColumnLayout {
        id: contentZone
        spacing: 0
        height: applicationWindow().height * 2
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Item {
            Layout.fillWidth: true
            implicitHeight: applicationWindow().height

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: root.contentToPlayerSpacing
                color: Kirigami.Theme.backgroundColor
            }

            Kirigami.Separator {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: minimizedPlayer.top
            }

            MinimizedVideoPlayer {
                id: minimizedPlayer
                visible: root.isVideoLoaded
                height: root.miniPlayerHeight
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: root.contentToPlayerSpacing

                previewSource: root.previewSource
                videoName: videoPlayer.videoName
                channelName: videoPlayer.channelName

                onToggleRequested: PlasmaTube.videoController.togglePlaying()
                onStopRequested: PlasmaTube.videoController.stop()
                onOpenRequested: root.open()
                onNextRequested: PlasmaTube.videoController.next()
            }
        }

        VideoPlayer {
            id: videoPlayer
            Layout.fillWidth: true
            Layout.fillHeight: true

            onRequestClosePlayer: root.close();
        }
    }
}
