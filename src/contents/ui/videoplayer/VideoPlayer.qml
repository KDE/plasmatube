// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.4 as Kirigami
import QtMultimedia 5.15
import QtGraphicalEffects 1.15

import org.kde.plasmatube 1.0
import org.kde.plasmatube.models 1.0
import "../utils.js" as Utils

import "../"

Kirigami.ScrollablePage {
    id: root

    property string vid: ""
    property var previewSource: renderer

    readonly property string videoName: videoModel.video.title
    readonly property string channelName: videoModel.video.author

    readonly property bool isPlaying: renderer.playbackState === MediaPlayer.PlayingState

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

    title: videoName
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    ColumnLayout {
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

        Item {
            id: videoContainer
            Layout.fillWidth: true
            Layout.preferredHeight: width / 16.0 * 9.0
            Layout.maximumHeight: root.height

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
            }

            VideoControls {
                anchors.fill: parent
                audio: playMusic
                video: renderer
            }

            Audio {
                id: playMusic
                source: videoModel.audioUrl
            }
        }


        // extra layout to make all details invisible while loading
        ColumnLayout {
            Layout.topMargin: Kirigami.Units.gridUnit
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.fillWidth: true
            visible: !videoModel.isLoading && videoModel.video.isLoaded
            enabled: !videoModel.isLoading && videoModel.video.isLoaded

            // title
            Kirigami.Heading {
                Layout.fillWidth: true
                text: videoModel.video.title
                wrapMode: Text.WordWrap
            }

            // author info and like statistics
            RowLayout {
                Layout.fillWidth: true
                Image {
                    Layout.preferredHeight: 50
                    Layout.preferredWidth: 50
                    fillMode: Image.PreserveAspectFit
                    source: videoModel.video.authorThumbnail(100)
                }

                ColumnLayout {
                    SubscriptionController {
                        id: subscriptionController
                        channelId: videoModel.video.authorId
                    }

                    Controls.Label {
                        text: videoModel.video.author
                        color: mouseArea.containsMouse ? Kirigami.Theme.highlightColor: Kirigami.Theme.textColor
                        MouseArea {
                            id: mouseArea
                            cursorShape: Qt.PointingHandCursor
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                const author = videoModel.video.author
                                const authorId = videoModel.video.authorId
                                pageStack.pop()
                                pageStack.push("qrc:/ChannelPage.qml", {author, authorId})
                            }
                        }
                    }
                    Controls.Button {
                        Layout.preferredWidth: subscribeButtonContent.width + Kirigami.Units.largeSpacing * 2
                        Kirigami.Theme.backgroundColor: subscriptionController.isSubscribed ? undefined : "red"
                        Kirigami.Theme.textColor: subscriptionController.isSubscribed ? undefined : "white"

                        RowLayout {
                            id: subscribeButtonContent
                            anchors.centerIn: parent
                            spacing: 0

                            Controls.BusyIndicator {
                                visible: subscriptionController.isLoading
                                Layout.preferredHeight: 34
                                Layout.preferredWidth: 34
                            }

                            Controls.Label {
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
                                showPassiveNotification(qsTr("Please log in to subscribe to channels."));
                                pageStack.layers.push("qrc:/AccountPage.qml");
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                ColumnLayout {
                    spacing: 0
                    Controls.Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: Utils.formatCount(videoModel.video.viewCount) + " views"
                    }
                    Controls.ProgressBar {
                        Layout.preferredWidth: 150
                        value: videoModel.video.rating / 5.0
                    }
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Kirigami.Icon {
                            source: "draw-arrow-up"
                            width: 22
                            height: 22
                        }
                        Controls.Label {
                            text: Utils.formatCount(videoModel.video.likeCount)
                        }

                        // placeholder
                        Item {
                            width: 5
                        }

                        Kirigami.Icon {
                            source: "draw-arrow-down"
                            width: 22
                            height: 22
                        }
                        Controls.Label {
                            text: Utils.formatCount(videoModel.video.dislikeCount)
                        }
                    }
                }
            }

            // video description
            Controls.Label {
                Layout.fillWidth: true
                text: videoModel.video.description
                wrapMode: Text.WordWrap
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
                    width: parent.width
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
