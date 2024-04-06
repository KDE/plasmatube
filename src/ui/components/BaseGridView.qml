// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import Qt.labs.qmlmodels

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

import org.kde.plasmatube

GridView {
    id: gridView

    // Optional, used on the playlist page to play the video
    property string playlistId

    property string currentVideoId
    property int currentVideoIndex
    property string currentVideoTitle
    property string currentChannelName
    property string currentChannelId

    readonly property real effectiveWidth: width - leftMargin - rightMargin
    readonly property real targetDelegateWidth: Kirigami.Units.gridUnit * 14 + Kirigami.Units.largeSpacing * 2
    readonly property int columns: Math.floor(effectiveWidth / targetDelegateWidth)

    topMargin: gridView.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
    bottomMargin: gridView.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
    leftMargin: gridView.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing
    rightMargin: gridView.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing

    currentIndex: -1
    // TODO: look into this assumption if we ever want to use gridview in the video player
    enabled: !applicationWindow().videoPlayerOpen

    keyNavigationEnabled: true
    cellWidth: effectiveWidth / columns
    cellHeight: (cellWidth / 16 * 9) + Kirigami.Units.gridUnit * 5 + Kirigami.Units.mediumSpacing

    Connections {
        target: model

        function onErrorOccured(errorText) {
            message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your proxy settings.", errorText);
            message.visible = true;
        }
    }

    delegate: DelegateChooser {
        role: "type"

        DelegateChoice {
            roleValue: "video"

            VideoGridItem {
                id: videoItem

                width: gridView.cellWidth
                height: gridView.cellHeight

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
                    videoModel.markAsWatched(index);
                    if (root.playlistId?.length > 0) {
                        PlasmaTube.videoController.videoQueue.loadPlaylistAndPlay(root.playlistId, vid);
                    } else {
                        PlasmaTube.videoController.play(vid);
                    }
                }

                onContextMenuRequested: {
                    currentVideoId = vid;
                    currentVideoIndex = index;
                    currentVideoTitle = title;
                    currentChannelName = author;
                    currentChannelId = authorId;
                    videoMenu.isWatched = watched;
                    // If we don't set a position, QtQuick just throws it anywhere on the screen with a touch input
                    if (Kirigami.Settings.hasTransientTouchInput) {
                        videoMenu.popup(videoItem, videoItem.width / 2, videoItem.height / 2);
                    } else {
                        videoMenu.popup();
                    }
                }
            }
        }
        DelegateChoice {
            roleValue: "channel"

            ChannelGridItem {
                width: gridView.cellWidth
                height: gridView.cellHeight

                name: model.channelName
                avatarUrl: model.channelAvatar

                onClicked: {
                    pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "ChannelPage"), {author: model.channelName, authorId: model.id});
                }
            }
        }
        DelegateChoice {
            roleValue: "playlist"

            PlaylistGridItem {
                width: gridView.cellWidth
                height: gridView.cellHeight

                title: model.title
                thumbnail: model.thumbnail
                videoCount: model.videoCount

                onClicked: {
                    pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "PlaylistPage"), {playlistId: model.id, title: model.title})
                }
            }
        }
    }

    footer: ColumnLayout {
        width: parent.width
        spacing: 0
        visible: gridView.count > 0

        QQC2.ProgressBar {
            id: loadingBar

            visible: gridView.model.isLoading
            indeterminate: true

            Layout.alignment: Qt.AlignHCenter
        }
    }

    Kirigami.LoadingPlaceholder {
        anchors.centerIn: parent
        visible: gridView.model.isLoading && gridView.count === 0
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        text: i18n("No videos")
        visible: !gridView.model.isLoading && gridView.count === 0
    }

    VideoMenu {
        id: videoMenu

        videoId: currentVideoId
        channelName: currentChannelName
        channelId: currentChannelId

        onMarkWatched: videoModel.markAsWatched(currentVideoIndex)
        onMarkUnwatched: videoModel.markAsUnwatched(currentVideoIndex)

        onAddToPlaylist: applicationWindow().openAddToPlaylistMenu()
    }

    Component.onCompleted: videoModel?.requestQuery(initialQuery, true)
}