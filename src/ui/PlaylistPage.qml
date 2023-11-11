// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

import org.kde.plasmatube

Kirigami.ScrollablePage {
    required property string playlistId
    property string currentVideoId
    property int currentVideoIndex

    id: root
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    supportsRefreshing: true
    onRefreshingChanged: {
        if (refreshing && !videoModel.isLoading) {
            videoModel.refresh();
        }
    }

    header: Components.Banner {
        id: message
        type: Kirigami.MessageType.Error
        width: parent.width

        showCloseButton: true

        actions: Kirigami.Action {
            text: i18n("Proxy Settings")
            icon.name: "settings-configure"
            onTriggered: pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "NetworkProxyPage"))
        }
    }

    actions: Kirigami.Action {
        text: i18nc("@action:button Play the playlist", "Play")
        icon.name: "media-playback-start"
        onTriggered: PlasmaTube.videoController.videoQueue.loadPlaylist(root.playlistId)
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    GridView {
        id: gridView
        topMargin: root.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
        bottomMargin: root.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
        leftMargin: root.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing
        rightMargin: root.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing

        readonly property real effectiveWidth: width - leftMargin - rightMargin
        readonly property real targetDelegateWidth: Kirigami.Units.gridUnit * 14 + Kirigami.Units.largeSpacing * 2
        readonly property int columns: Math.floor(effectiveWidth / targetDelegateWidth)

        cellWidth: effectiveWidth / columns
        cellHeight: (cellWidth / 16 * 9) + Kirigami.Units.gridUnit * 4

        currentIndex: -1
        model: VideoListModel {
            id: videoModel
            onIsLoadingChanged: {
                root.refreshing = isLoading
            }
            onErrorOccured: (errorText) => {
                message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your proxy settings.", errorText);
                message.visible = true;
            }
        }
        delegate: VideoGridItem {
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

            onPressed: {
                videoModel.markAsWatched(index);
                PlasmaTube.videoController.play(vid);
            }

            onContextMenuRequested: {
                currentVideoId = vid;
                currentVideoIndex = index;
                videoMenu.isWatched = watched;
                videoMenu.popup();
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: gridView.count === 0 && !root.refreshing
            text: i18nc("@info:status", "Loading…")
        }
    }

    VideoMenu {
        id: videoMenu

        videoId: currentVideoId
        shouldRemoveFromPlaylist: true

        onMarkWatched: videoModel.markAsWatched(currentVideoIndex)
        onMarkUnwatched: videoModel.markAsUnwatched(currentVideoIndex)

        onRemoveFromPlaylist: videoModel.removeFromPlaylist(root.playlistId, root.currentVideoIndex)
    }

    Component.onCompleted: videoModel.requestPlaylist(playlistId)
}
