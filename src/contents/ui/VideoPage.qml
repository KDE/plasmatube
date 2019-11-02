/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.4 as Kirigami

import org.kde.plasmatube.mpv 1.0
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

    VideoModel {
        id: videoModel
        videoId: vid
        onVideoIdChanged: {
            // is also executed in initial set
            videoModel.fetch()
        }
    }

    ColumnLayout {
        MpvObject {
            id: renderer
            Layout.preferredWidth: root.width
            Layout.preferredHeight: root.width / 16.0 * 9.0
            Layout.maximumHeight: root.height

            MouseArea {
                anchors.fill: parent
                onClicked: renderer.command(["cycle", "pause"])
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

                    property bool isSubscribed: AccountManager.subscribedChanneldIds.indexOf(videoModel.video.authorId) != -1

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

                            onSubscribedToChannel: subscribingIndicator.visible = false;
                            onSubscribingFailed: {
                                subscribingIndicator.visible = false;
                                showPassiveNotification(qsTr("Could not subscribe to channel."));
                            }
                            onUnsubscribedFromChannel: subscribingIndicator.visible = false;
                            onUnsubscribingFailed: {
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

    Component.onCompleted: {
        renderer.command(["loadfile", "ytdl://" + vid])
    }
}
