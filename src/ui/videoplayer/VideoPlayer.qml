// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects

import org.kde.plasmatube
import "../components/utils.js" as Utils
import "../"
import "../components"

Kirigami.ScrollablePage {
    id: root

    flickable.boundsBehavior: Flickable.StopAtBounds

    title: videoName
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    readonly property var video: PlasmaTube.videoController.currentVideo

    property var previewSource: renderer

    property string currentVideoId
    property int currentVideoIndex
    property string currentVideoTitle

    readonly property string videoName: video.title
    readonly property string channelName: video.author
    readonly property bool isPlaying: !renderer.paused

    readonly property bool widescreen: root.width > 1200
    property bool inFullScreen: false

    signal requestClosePlayer()

    function startStop() {
        if (PlasmaTube.videoController.currentPlayer.paused) {
            if (root.atEnd) {
                PlasmaTube.videoController.currentPlayer.setPosition(0);
            }

            PlasmaTube.videoController.currentPlayer.play();
        } else {
            PlasmaTube.videoController.currentPlayer.pause();
        }
    }

    function goToChannel() {
        const author = video.author;
        const authorId = video.authorId;
        pageStack.pop();
        pageStack.push(Qt.createComponent("org.kde.plasmatube", "ChannelPage"), {author, authorId});
        root.requestClosePlayer();
    }

    function toggleFullscreen() {
        if (root.inFullScreen) {
            root.exitFullScreen();
        } else {
            root.openFullScreen();
        }
    }

    function openFullScreen() {
        videoContainer.parent = QQC2.Overlay.overlay;
        videoContainer.anchors.fill = QQC2.Overlay.overlay;
        root.inFullScreen = true;
        PlasmaTube.setInhibitSleep(true);
        applicationWindow().showFullScreen();
    }

    function exitFullScreen() {
        videoContainer.parent = inlineVideoContainer;
        videoContainer.anchors.fill = inlineVideoContainer;
        root.inFullScreen = false;
        PlasmaTube.setInhibitSleep(false);
        applicationWindow().showNormal();
    }

    header: Kirigami.AbstractApplicationHeader{
        contentItem:
            RowLayout{
                QQC2.ToolButton {
                id: closeButton
                icon.name: "go-previous-view"

                width: Kirigami.Units.gridUnit * 2
                height: Kirigami.Units.gridUnit * 2
                onClicked: root.requestClosePlayer();

            }
            Kirigami.Heading{
                text: video.title
            }
        }
    }
    GridLayout {
        columns: root.widescreen ? 2 : 1
        rowSpacing: 0
        columnSpacing: 0

        ColumnLayout {
            id: parentColumn
            spacing: 0
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true

            Item {
                Layout.margins: widescreen? Kirigami.Units.largeSpacing * 2 : 0
                id: inlineVideoContainer
                Layout.fillWidth: true
                Layout.preferredHeight: width / 16.0 * 9.0
                Layout.maximumHeight: root.height
                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: playerMask
                }
                MouseArea {
                    id: videoContainer
                    anchors.fill: parent

                    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                    Kirigami.Theme.inherit: false

                    property bool showControls: false
                    onEntered: {
                        if (!showControls) {
                            showControls = true;
                            controlTimer.restart();
                        }
                    }
                    onExited: {
                        if (showControls) {
                            controlTimer.stop();
                            videoContainer.showControls = false;
                        }
                    }
                    onClicked: root.startStop()
                    onDoubleClicked: root.toggleFullscreen()

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
                    Rectangle {
                        id: playerMask
                        radius: widescreen ? 7 : 0
                        anchors.fill: renderer
                        visible: false
                    }
                    VideoData {
                        title: video.title
                        visible: opacity > 0
                        opacity: videoContainer.showControls ? 1 : 0
                        Behavior on opacity {
                            NumberAnimation { duration: Kirigami.Units.veryLongDuration; easing.type: Easing.InOutCubic }
                        }
                    }

                    VideoControls {
                        inFullScreen: root.inFullScreen
                        anchors.fill: parent

                        onRequestFullScreen: root.toggleFullscreen()
                        onStartStop: root.startStop()

                        visible: opacity > 0
                        opacity: videoContainer.showControls ? 1 : 0
                        Behavior on opacity {
                            NumberAnimation { duration: Kirigami.Units.veryLongDuration; easing.type: Easing.InOutCubic }
                        }
                    }
                }
            }


            // extra layout to make all details invisible while loading
            ColumnLayout {
                Layout.topMargin: Kirigami.Units.gridUnit
                Layout.leftMargin: Kirigami.Units.gridUnit
                Layout.rightMargin: Kirigami.Units.gridUnit
                Layout.fillWidth: true
                spacing: 0
                visible: !PlasmaTube.videoController.videoModel.isLoading && video.isLoaded
                enabled: !PlasmaTube.videoController.videoModel.isLoading && video.isLoaded

                // title
                Kirigami.Heading {
                    Layout.fillWidth: true
                    text: video.title
                    wrapMode: Text.Wrap
                    font.weight: Font.Bold
                }

                // author info and like statistics
                RowLayout {
                    Layout.topMargin: Kirigami.Units.gridUnit
                    Layout.fillWidth: true
                    spacing: Kirigami.Units.largeSpacing

                    Image {
                        id: chanelThumb
                        Layout.preferredHeight: 50
                        Layout.preferredWidth: 50
                        fillMode: Image.PreserveAspectFit
                        source: video.authorThumbnail(100)
                        layer.enabled: true
                        layer.effect: OpacityMask {
                            maskSource: mask
                        }
                        Rectangle {
                            id: mask
                            radius: chanelThumb.height/2
                            anchors.fill: chanelThumb
                            visible: false
                        }
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.goToChannel()
                        }
                    }

                    ColumnLayout {
                        id: column
                        spacing: 0

                        QQC2.Label {
                            text: video.author
                            font.weight: Font.Bold

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: root.goToChannel()
                            }
                        }

                        SubscriptionButton {
                            id: subscribeButton

                            channelId: video.authorId
                            subCountText: video.subCountText
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Layout.topMargin: Kirigami.Units.gridUnit
                    spacing: Kirigami.Units.largeSpacing

                    Kirigami.Chip {
                        closable: false
                        enabled: false
                        labelItem.color: Kirigami.Theme.disabledTextColor
                        labelItem.font.weight: Font.Bold
                        text: i18n("%1 views", Utils.formatCount(video.viewCount))
                    }

                    Kirigami.Chip {
                        closable: false
                        enabled: false
                        labelItem.color: Kirigami.Theme.disabledTextColor
                        labelItem.font.weight: Font.Bold
                        text: i18n("%1 Likes", Utils.formatCount(video.likeCount))
                    }
                }

                // video description
                QQC2.TextArea {
                    readonly property var linkRegex: /(href=["'])?(\b(https?):\/\/[^\s\<\>\"\'\\\?\:\)\(]+(\(.*?\))*(\?(?=[a-z])[^\s\\\)]+|$)?)/g

                    text: video.description.replace(linkRegex, function() {
                        if (arguments[1]) {
                            return arguments[0];
                        }
                        const l = arguments[2];
                        if ([".", ","].includes(l[l.length-1])) {
                            const link = l.substring(0, l.length-1);
                            const leftover = l[l.length-1];
                            return `<a href="${link}">${link}</a>${leftover}`;
                        }
                        return `<a href="${l}">${l}</a>`;
                    }).replace(/(?:\r\n|\r|\n)/g, '<br>')
                    textFormat: TextEdit.RichText
                    background: null
                    readOnly: true
                    padding: 0

                    Layout.topMargin: Kirigami.Units.gridUnit
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true

                    onHoveredLinkChanged: if (hoveredLink.length > 0 && hoveredLink !== "1") {
                        applicationWindow().hoverLinkIndicator.text = hoveredLink;
                    } else {
                        applicationWindow().hoverLinkIndicator.text = "";
                    }

                    onLinkActivated: (link) => Qt.openUrlExternally(link)

                    HoverHandler {
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.IBeamCursor
                    }

                    selectByMouse: !Kirigami.Settings.isMobile
                }

                Comments {
                    id: comments
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }

            QQC2.BusyIndicator {
                Layout.alignment: Qt.AlignCenter
                visible: PlasmaTube.videoController.videoModel.isLoading
            }
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: !root.widescreen
            Layout.preferredWidth: Kirigami.Units.gridUnit * 20
            spacing: 0

            Kirigami.Heading {
                text: i18n("Queue")
            }

            VideoQueueView {
                Layout.fillWidth: true
                Layout.preferredHeight: Kirigami.Units.gridUnit * 15
            }

            Kirigami.Heading {
                text: i18n("Recommended")
            }

            Repeater {
                model: video.recommendedVideosModel()
                delegate: VideoListItem {
                    id: videoDelegate
                    Layout.fillWidth: true
                    Layout.maximumWidth: parentColumn.width
                    vid: model.id
                    thumbnail: model.thumbnail
                    liveNow: model.liveNow
                    length: model.length
                    title: model.title
                    author: model.author
                    authorId: model.authorId
                    description: model.description
                    viewCount: model.viewCount
                    publishedText: model.publishedText
                    watched: model.watched

                    onPressed: {
                        video.recommendedVideosModel().markAsWatched(index);
                        PlasmaTube.videoController.play(vid);
                    }

                    onContextMenuRequested: {
                        currentVideoId = vid;
                        currentVideoIndex = index;
                        currentVideoTitle = title;
                        videoMenu.isWatched = watched;
                        videoMenu.popup();
                    }
                }
            }
        }
    }

    VideoMenu {
        id: videoMenu

        videoId: currentVideoId

        onMarkWatched: video.recommendedVideosModel().markAsWatched(currentVideoIndex)
        onMarkUnwatched: video.recommendedVideosModel().markAsUnwatched(currentVideoIndex)
    }

    Connections {
        target: PlasmaTube.videoController

        function onCurrentVideoChanged() {
            if (PlasmaTube.videoController.currentVideo !== null) {
                comments.loadComments(PlasmaTube.videoController.currentVideo.videoId);
            }
        }
    }
}
