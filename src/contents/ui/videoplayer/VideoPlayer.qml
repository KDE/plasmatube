// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.19 as Kirigami
import QtMultimedia 5.15
import QtGraphicalEffects 1.15

import org.kde.plasmatube 1.0
import org.kde.plasmatube.models 1.0
import "../utils.js" as Utils

import "../"

Kirigami.ScrollablePage {
    id: root
    flickable.boundsBehavior: Flickable.StopAtBounds

    property string vid: ""
    property var previewSource: renderer

    readonly property string videoName: videoModel.video.title
    readonly property string channelName: videoModel.video.author

    readonly property bool isPlaying: renderer.playbackState === MediaPlayer.PlayingState

    signal requestClosePlayer()

    function stop() {
        vid = "";
        renderer.pause();
        playMusic.pause();
    }

    function togglePlaying() {
        if (renderer.playbackState !== MediaPlayer.PlayingState) {
            renderer.play();
            playMusic.play();
        } else {
            renderer.pause();
            playMusic.pause();
        }
    }

    function goToChannel() {
        const author = videoModel.video.author;
        const authorId = videoModel.video.authorId;
        pageStack.pop();
        pageStack.push("qrc:/ChannelPage.qml", {author, authorId});
        root.requestClosePlayer();
    }

    title: videoName
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    ColumnLayout {
        id: parentColumn
        spacing: 0

        VideoModel {
            id: videoModel
            videoId: vid
            onVideoIdChanged: {
                // is also executed in initial set
                videoModel.fetch()
            }
            onErrorOccurred: (errorText) => {
                applicationWindow().showPassiveNotification(errorText)
            }
        }

        MouseArea {
            id: videoContainer
            Layout.fillWidth: true
            Layout.preferredHeight: width / 16.0 * 9.0
            Layout.maximumHeight: root.height

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

            Video {
                id: renderer
                anchors.fill: parent
                property var video: videoModel
                source: videoModel.remoteUrl
                muted: true

                Image {
                    anchors.fill: parent
                    visible: renderer.playbackState === MediaPlayer.StoppedState
                    source: videoModel.video.thumbnailUrl("high")
                }
            }

            VideoData {
                title: videoModel.video.title

                visible: opacity > 0
                opacity: videoContainer.showControls ? 1 : 0
                Behavior on opacity {
                    NumberAnimation { duration: Kirigami.Units.veryLongDuration; easing.type: Easing.InOutCubic }
                }
            }

            VideoControls {
                anchors.fill: parent
                audio: playMusic
                video: renderer

                visible: opacity > 0
                opacity: videoContainer.showControls ? 1 : 0
                Behavior on opacity {
                    NumberAnimation { duration: Kirigami.Units.veryLongDuration; easing.type: Easing.InOutCubic }
                }
            }

            Audio {
                id: playMusic
                source: videoModel.audioUrl
            }

            Controls.ToolButton {
                id: closeButton
                anchors.top: parent.top
                anchors.topMargin: Kirigami.Units.largeSpacing
                anchors.left: parent.left
                anchors.leftMargin: Kirigami.Units.largeSpacing
                icon.name: "go-previous-view"
                icon.width: Kirigami.Units.iconSizes.small
                icon.height: Kirigami.Units.iconSizes.small

                width: Kirigami.Units.gridUnit * 2
                height: Kirigami.Units.gridUnit * 2
                onClicked: root.requestClosePlayer();

                TabIndicator {}
            }
        }


        // extra layout to make all details invisible while loading
        ColumnLayout {
            Layout.topMargin: Kirigami.Units.gridUnit
            Layout.leftMargin: Kirigami.Units.gridUnit
            Layout.rightMargin: Kirigami.Units.gridUnit
            Layout.fillWidth: true
            spacing: 0
            visible: !videoModel.isLoading && videoModel.video.isLoaded
            enabled: !videoModel.isLoading && videoModel.video.isLoaded

            // title
            Kirigami.Heading {
                Layout.fillWidth: true
                text: videoModel.video.title
                wrapMode: Text.Wrap
                font.weight: Font.Bold
            }

            // author info and like statistics
            RowLayout {
                Layout.topMargin: Kirigami.Units.gridUnit
                Layout.fillWidth: true
                spacing: Kirigami.Units.largeSpacing

                Image {
                    Layout.preferredHeight: 50
                    Layout.preferredWidth: 50
                    fillMode: Image.PreserveAspectFit
                    source: videoModel.video.authorThumbnail(100)

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.goToChannel()
                    }
                }

                ColumnLayout {
                    id: column
                    spacing: 0

                    SubscriptionController {
                        id: subscriptionController
                        channelId: videoModel.video.authorId
                    }

                    Controls.Label {
                        text: videoModel.video.author
                        font.weight: Font.Bold

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.goToChannel()
                        }
                    }

                    Controls.Button {
                        id: subscribeButton
                        Layout.topMargin: Kirigami.Units.smallSpacing

                        background: Rectangle {
                            color: subscriptionController.isSubscribed ? "grey" : "red"
                            radius: Kirigami.Units.smallSpacing

                            Rectangle {
                                anchors.fill: parent
                                radius: Kirigami.Units.smallSpacing
                                color: "black"
                                opacity: subscribeButton.pressed ? 0.6 : subscribeButton.hovered ? 0.3 : 0
                            }
                        }

                        leftPadding: Kirigami.Units.largeSpacing
                        rightPadding: Kirigami.Units.largeSpacing
                        bottomPadding: Kirigami.Units.largeSpacing
                        topPadding: Kirigami.Units.largeSpacing

                        contentItem: RowLayout {
                            id: subscribeButtonContent
                            spacing: 0

                            Controls.BusyIndicator {
                                visible: subscriptionController.isLoading
                                Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                                Layout.preferredWidth: Kirigami.Units.gridUnit * 2
                            }

                            Controls.Label {
                                color: "white"
                                text: {
                                    if (subscriptionController.isSubscribed) {
                                        return "Unsubscribe (" + videoModel.video.subCountText + ")"
                                    }
                                    return "Subscribe (" + videoModel.video.subCountText + ")"
                                }
                            }
                        }

                        onClicked: {
                            if (subscriptionController.canToggleSubscription()) {
                                subscriptionController.toggleSubscription()
                            } else if (!PlasmaTube.isLoggedIn) {
                                root.requestClosePlayer();
                                showPassiveNotification(qsTr("Please log in to subscribe to channels."));
                                pageStack.layers.push("qrc:/AccountPage.qml");
                            }
                        }
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
                    labelItem.color: Kirigami.Theme.disabledTextColor
                    labelItem.font.weight: Font.Bold
                    text: i18n("%1 views", Utils.formatCount(videoModel.video.viewCount))
                }

                Kirigami.Chip {
                    closable: false
                    labelItem.color: Kirigami.Theme.disabledTextColor
                    labelItem.font.weight: Font.Bold
                    text: i18n("%1 Likes", Utils.formatCount(videoModel.video.likeCount))
                }
            }

            // video description
            Controls.Label {
                Layout.topMargin: Kirigami.Units.gridUnit
                Layout.bottomMargin: Kirigami.Units.largeSpacing
                Layout.fillWidth: true
                text: videoModel.video.description
                wrapMode: Text.Wrap
            }
        }

        Controls.BusyIndicator {
            Layout.alignment: Qt.AlignCenter
            visible: videoModel.isLoading
        }

        ColumnLayout {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            spacing: 0

            Repeater {
                model: videoModel.video.recommendedVideosModel()
                delegate: VideoListItem {
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

                    onClicked: {
                        root.vid = model.id;
                    }
                }
            }
        }
    }
}
