// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.components as Components

import org.kde.plasmatube
import "components"

Kirigami.ScrollablePage {
    id: root
    title: videoModel.title
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    property var initialQuery: VideoListModel.Trending
    property alias videoModel: videoModel
    property string currentVideoId
    property int currentVideoIndex
    property string currentVideoTitle

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

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    GridView {
        id: gridView
        topMargin: root.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
        bottomMargin: root.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
        leftMargin: root.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing
        rightMargin: root.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing

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

        readonly property real effectiveWidth: width - leftMargin - rightMargin
        readonly property real targetDelegateWidth: Kirigami.Units.gridUnit * 14 + Kirigami.Units.largeSpacing * 2
        readonly property int columns: Math.floor(effectiveWidth / targetDelegateWidth)

        keyNavigationEnabled: true
        cellWidth: effectiveWidth / columns
        cellHeight: (cellWidth / 16 * 9) + Kirigami.Units.gridUnit * 4

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
                currentVideoTitle = title;
                videoMenu.isWatched = watched;
                videoMenu.popup();
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
    }

    VideoMenu {
        id: videoMenu

        videoId: currentVideoId

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

                    delegate: Delegates.RoundedItemDelegate
                        {
                        required property string playlistId
                        required property string title

                        text: title

                        Layout.fillWidth: true

                        onClicked: {
                            PlasmaTube.selectedSource.addToPlaylist(playlistId, currentVideoId);
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

