// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.4 as Kirigami
import QtMultimedia 5.15

import org.kde.plasmatube.models 1.0
import org.kde.plasmatube.accountmanager 1.0
import "utils.js" as Utils

Kirigami.ScrollablePage {
    id: root
    property string vid

    title: videoModel.video.title
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    ColumnLayout {
        VideoModel {
            id: videoModel
            videoId: vid
            onVideoIdChanged: {
                // is also executed in initial set
                videoModel.fetch()
            }
        }
        Video {
            id: renderer
            Layout.fillWidth: true
            Layout.preferredHeight: width / 16.0 * 9.0
            Layout.maximumHeight: root.height
            source: videoModel.remoteUrl
            muted: true

            Image {
                anchors.fill: parent
                visible: renderer.playbackState === MediaPlayer.StoppedState
                source: videoModel.video.thumbnailUrl("high")
            }

            VideoData {
                title: videoModel.video.title
            }

            VideoControls {
                audio: playMusic
                video: renderer
            }

            /*MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (renderer.playbackState !== MediaPlayer.PlayingState) {
                        renderer.play()
                        playMusic.play();
                    } else {
                        renderer.pause()
                        playMusic.pause()
                    }
                }
            }*/

            Audio {
                id: playMusic
                source: videoModel.audioUrl
            }
        }


        // extra layout to make all details invisible while loading
        ColumnLayout {
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.fillWidth: true
            visible: !videoModel.isLoading
            enabled: !videoModel.isLoading

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
                    id: authorView

                    property bool isSubscribed: AccountManager.subscribedChanneldIds.indexOf(videoModel.video.authorId) !== -1

                    Controls.Label {
                        text: videoModel.video.author
                    }
                    Controls.Button {
                        Layout.preferredWidth: subscribeButtonContent.width + Kirigami.Units.largeSpacing * 2
                        Kirigami.Theme.backgroundColor: authorView.isSubscribed ? undefined : "red"
                        Kirigami.Theme.textColor: authorView.isSubscribed ? undefined : "white"

                        RowLayout {
                            id: subscribeButtonContent
                            anchors.centerIn: parent
                            spacing: 0

                            Controls.BusyIndicator {
                                id: subscribingIndicator
                                visible: false
                                Layout.preferredHeight: 34
                                Layout.preferredWidth: 34
                            }

                            Controls.Label {
                                text: {
                                    if (authorView.isSubscribed)
                                        return "Unsubscribe (" + videoModel.video.subCountText + ")";
                                    return "Subscribe (" + videoModel.video.subCountText + ")";
                                }
                            }
                        }

                        onClicked: {
                            if (AccountManager.username.length > 0) {
                                if (authorView.isSubscribed)
                                    AccountManager.unsubscribeFromChannel(videoModel.video.authorId);
                                else
                                    AccountManager.subscribeToChannel(videoModel.video.authorId);
                                subscribingIndicator.visible = true;
                            } else {
                                showPassiveNotification(qsTr("Please log in to subscribe to channels."));
                                pageStack.layers.push(loginPageComponent);
                            }
                        }

                        Connections {
                            target: AccountManager

                            function onSubscribedToChannel() {
                                subscribingIndicator.visible = false;
                            }
                            function onSubscribingFailed() {
                                subscribingIndicator.visible = false;
                                showPassiveNotification(qsTr("Could not subscribe to channel."));
                            }
                            function onUnsubscribedFromChannel() {
                                subscribingIndicator.visible = false;
                            }
                            function onUnsubscribingFailed() {
                                subscribingIndicator.visible = false;
                                showPassiveNotification(qsTr("Could not unsubscribe from channel."));
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
