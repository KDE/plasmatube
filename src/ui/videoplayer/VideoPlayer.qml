// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects

import org.kde.plasmatube
import "../components/utils.js" as Utils
import "../components"

/*
 * The actual video player page. Contains the video controls, information and other gubbins.
 */
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
    property string currentVideoUrl
    property string currentChannelName
    property string currentChannelId

    readonly property string videoName: video.title
    readonly property string channelName: video.author
    readonly property bool isPlaying: !renderer.paused

    readonly property bool widescreen: root.width > 1200
    property bool inFullScreen: false
    property int oldVisibility

    signal requestClosePlayer()

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
        videoPlayer.parent = QQC2.Overlay.overlay;
        videoPlayer.anchors.fill = QQC2.Overlay.overlay;
        root.inFullScreen = true;
        applicationWindow().globalDrawer.close();
        root.oldVisibility = applicationWindow().visibility;
        applicationWindow().showFullScreen();
    }

    function exitFullScreen() {
        videoPlayer.parent = videoContainer;
        videoPlayer.anchors.fill = videoContainer;
        root.inFullScreen = false;
        if (!applicationWindow().globalDrawer.modal) {
            applicationWindow().globalDrawer.open();
        }
        if (root.oldVisibility === Window.Maximized) {
            // You need to show the window normally, then go maximized.
            applicationWindow().showNormal();
            applicationWindow().showMaximized();
        } else {
            applicationWindow().showNormal();
        }
    }

    header: Kirigami.AbstractApplicationHeader {
        contentItem: RowLayout {
            anchors {
                left: parent.left
                leftMargin: Kirigami.Units.smallSpacing
                right: parent.right
                rightMargin: Kirigami.Units.smallSpacing
            }

            spacing: Kirigami.Units.smallSpacing

            QQC2.ToolButton {
                id: closeButton
                icon.name: "arrow-down-symbolic"
                text: i18nc("@action:button", "Hide player")
                display: QQC2.AbstractButton.IconOnly

                Layout.preferredWidth: Kirigami.Units.gridUnit * 2
                Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                Layout.leftMargin: applicationWindow().globalDrawer.modal ? width : 0

                onClicked: root.requestClosePlayer();

                QQC2.ToolTip.text: text
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }

            Kirigami.ActionToolBar {
                Layout.alignment: Qt.AlignRight
                alignment: Qt.AlignRight

                actions: [
                    Kirigami.Action {
                        id: addToPlaylistAction

                        text: i18nc("@action:button", "Add to Playlist…")
                        icon.name: "media-playlist-append"
                        visible: PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn

                        onTriggered: applicationWindow().openAddToPlaylistMenu(root.video.videoId)
                    },
                    ShareAction {
                        id: shareAction

                        inputData: {
                            'urls': [root.video.url],
                            'title': video.title
                        }
                    }
                ]
            }
        }
    }

    Keys.onLeftPressed: (event) => {
        PlasmaTube.videoController.currentPlayer.seek(-5);
        event.accepted = true;
    }

    Keys.onRightPressed: (event) => {
        PlasmaTube.videoController.currentPlayer.seek(5);
        event.accepted = true;
    }

    Keys.onEscapePressed: (event) => {
        if (root.inFullScreen) {
            root.exitFullScreen();
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

            // This is the overall video container.
            // It's a container-inside-container detail, because... when we fullscreen a video it reparents the inner child.
            // If we didn't have an outside container, we wouldn't know where to put it back.
            Item {
                id: videoContainer

                Layout.margins: root.widescreen ? Kirigami.Units.largeSpacing * 2 : 0
                Layout.fillWidth: true
                Layout.preferredHeight: width / 16.0 * 9.0
                Layout.maximumHeight: root.height
                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: playerMask
                }

                Item {
                    id: videoPlayer

                    anchors.fill: parent

                    property bool showControls: false

                    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                    Kirigami.Theme.inherit: false

                    Timer {
                        id: controlTimer
                        interval: 2000
                        onTriggered: videoPlayer.showControls = false
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
                            source: root.video.thumbnailUrl("high")
                            fillMode: Image.PreserveAspectCrop
                        }

                        QQC2.BusyIndicator {
                            anchors.centerIn: parent
                        }
                    }
                    Rectangle {
                        id: playerMask
                        radius: root.widescreen ? 7 : 0
                        anchors.fill: renderer
                        visible: false
                    }
                    VideoControls {
                        inFullScreen: root.inFullScreen
                        anchors.fill: parent

                        onRequestFullScreen: root.toggleFullscreen()

                        visible: opacity > 0
                        opacity: videoPlayer.showControls ? 1 : 0
                        Behavior on opacity {
                            NumberAnimation { duration: Kirigami.Units.veryLongDuration; easing.type: Easing.InOutCubic }
                        }

                        Keys.forwardTo: [root]
                    }

                    HoverHandler {
                        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad

                        onHoveredChanged: {
                            if (hovered) {
                                videoPlayer.showControls = true;
                                controlTimer.stop();
                            } else {
                                controlTimer.start();
                            }
                        }
                    }

                    TapHandler {
                        onTapped: {
                            if (Kirigami.Settings.hasTransientTouchInput) {
                                videoPlayer.showControls = true;
                                controlTimer.restart();
                            }
                        }
                        onDoubleTapped: root.toggleFullscreen()
                    }
                }
            }

            // extra layout to make all details invisible while loading
            ColumnLayout {
                Layout.topMargin: root.widescreen ? 0 : Kirigami.Units.gridUnit
                Layout.leftMargin: Kirigami.Units.gridUnit
                Layout.rightMargin: Kirigami.Units.gridUnit
                Layout.fillWidth: true
                spacing: 0
                visible: !PlasmaTube.videoController.videoModel.isLoading && root.video.isLoaded
                enabled: !PlasmaTube.videoController.videoModel.isLoading && root.video.isLoaded

                // title
                Kirigami.Heading {
                    Layout.fillWidth: true
                    text: root.video.title
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
                        source: root.video.authorThumbnail(100)
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
                            text: root.video.author
                            font.weight: Font.Bold

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: root.goToChannel()
                            }
                        }

                        SubscriptionButton {
                            id: subscribeButton

                            channelId: root.video.authorId
                            subCount: root.video.subCount
                        }
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
                        text: root.video.publishedText
                    }

                    Kirigami.Chip {
                        closable: false
                        enabled: false
                        labelItem.color: Kirigami.Theme.disabledTextColor
                        labelItem.font.weight: Font.Bold
                        text: i18n("%1 views", Utils.formatCount(root.video.viewCount))
                    }

                    Kirigami.Chip {
                        closable: false
                        enabled: false
                        labelItem.color: Kirigami.Theme.disabledTextColor
                        labelItem.font.weight: Font.Bold
                        text: i18n("%1 Likes", Utils.formatCount(root.video.likeCount))
                        visible: root.video.likeCount > 0 // hide like count when we don't know/there is none
                    }
                }

                // video description
                QQC2.TextArea {
                    readonly property var linkRegex: /(href=["'])?(\b(https?):\/\/[^\s\<\>\"\'\\\?\:\)\(]+(\(.*?\))*(\?(?=[a-z])[^\s\\\)]+|$)?)/g

                    Layout.preferredHeight: root.video.description.length > 0 ? implicitHeight : 0

                    text: root.video.description.replace(linkRegex, function() {
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
                    wrapMode: Text.WordWrap

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

                Kirigami.Heading {
                    text: i18n("Comments")
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
            Layout.margins: Kirigami.Units.largeSpacing * 2
            Layout.fillWidth: !root.widescreen
            Layout.preferredWidth: Kirigami.Units.gridUnit * 20
            spacing: Kirigami.Units.largeSpacing

            VideoQueueView {
                Layout.fillWidth: true
                Layout.preferredHeight: Kirigami.Units.gridUnit * 15
                visible: PlasmaTube.videoController.videoQueue.shouldBeVisible
            }

            Kirigami.Heading {
                text: i18n("Recommended")
            }

            Repeater {
                model: root.video.recommendedVideosModel()
                delegate: VideoListItem {
                    id: videoDelegate

                    required property var model

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

                    onClicked: {
                        root.video.recommendedVideosModel().markAsWatched(videoDelegate.index);
                        PlasmaTube.videoController.play(vid);
                    }

                    onContextMenuRequested: {
                        currentVideoId = model.id;
                        currentVideoIndex = index;
                        currentVideoTitle = title;
                        currentVideoTitle = model.url;
                        currentChannelName = author;
                        currentChannelId = authorId;
                        videoMenu.isWatched = watched;
                        videoMenu.popup();
                    }
                }
            }
        }
    }

    VideoMenu {
        id: videoMenu

        videoId: root.currentVideoId
        videoTitle: root.currentVideoTitle
        videoUrl: root.currentVideoUrl
        channelName: root.currentChannelName
        channelId: root.currentChannelId

        onMarkWatched: root.video.recommendedVideosModel().markAsWatched(root.currentVideoIndex)
        onMarkUnwatched: root.video.recommendedVideosModel().markAsUnwatched(root.currentVideoIndex)

        onAddToPlaylist: applicationWindow().openAddToPlaylistMenu(root.currentVideoId)
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
