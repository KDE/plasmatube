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

    property string currentVideoId
    property int currentVideoIndex
    property string currentVideoTitle
    property string currentChannelName
    property string currentChannelId

    readonly property real effectiveWidth: width - leftMargin - rightMargin
    readonly property real targetDelegateWidth: Kirigami.Units.gridUnit * 14 + Kirigami.Units.largeSpacing * 2
    readonly property int columns: Math.floor(effectiveWidth / targetDelegateWidth)

    topMargin: root.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
    bottomMargin: root.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
    leftMargin: root.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing
    rightMargin: root.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing

    currentIndex: -1
    // TODO: look into this assumption if we ever want to use gridview in the video player
    enabled: !applicationWindow().videoPlayerOpen

    keyNavigationEnabled: true
    cellWidth: effectiveWidth / columns
    cellHeight: (cellWidth / 16 * 9) + Kirigami.Units.gridUnit * 5 + Kirigami.Units.mediumSpacing

    Connections {
        target: model

        function onIsLoadingChanged() {
            root.refreshing = gridView.model.isLoading;
        }

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
                    PlasmaTube.videoController.play(vid);
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
                    pageStack.push(Qt.createComponent("org.kde.plasmatube", "ChannelPage"), {author: model.channelName, authorId: model.id});
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
                    pageStack.push(Qt.createComponent("org.kde.plasmatube", "PlaylistPage"), {playlistId: model.id, title: model.title})
                }
            }
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        text: i18nc("@info:status", "Loading…")
        visible: gridView.model.isLoading
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

        onAddToPlaylist: {
            addToPlaylistLoader.active = true;
            addToPlaylistLoader.item.open();
        }
    }

    Loader {
        id: addToPlaylistLoader

        active: false
        sourceComponent: Kirigami.PromptDialog {
            id: addToPlaylistDialog

            title: i18n("Add to playlist")

            standardButtons: Kirigami.Dialog.NoButton

            mainItem: ColumnLayout {
                Repeater {
                    id: playlists

                    model: PlaylistsModel {
                        id: playlistsModel
                    }

                    delegate: QQC2.ItemDelegate {
                        text: model.title

                        Layout.fillWidth: true

                        onClicked: {
                            PlasmaTube.selectedSource.addToPlaylist(model.id, currentVideoId);
                            addToPlaylistDialog.close();
                        }
                    }
                }
            }
            onClosed: addToPlaylistLoader.active = false
        }
    }

    Component.onCompleted: videoModel.requestQuery(initialQuery)
}