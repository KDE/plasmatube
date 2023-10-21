// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.plasmatube 1.0
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
                applicationWindow().showPassiveNotification(errorText)
            }
        }

        readonly property real effectiveWidth: width - leftMargin - rightMargin
        readonly property real targetDelegateWidth: Kirigami.Units.gridUnit * 14 + Kirigami.Units.largeSpacing * 2
        readonly property int columns: Math.floor(effectiveWidth / targetDelegateWidth)

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

            onClicked: (mouse) => {
                if (mouse.button === Qt.LeftButton) {
                    applicationWindow().switchVideo(vid);
                    applicationWindow().openPlayer();
                } else {
                    currentVideoId = vid;
                    currentVideoIndex = index;
                    currentVideoTitle = title;
                    videoMenu.isWatched = PlasmaTube.isVideoWatched(vid);
                    videoMenu.popup();
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
    }

    QQC2.Menu {
        id: videoMenu

        modal: true

        property bool isWatched

        QQC2.MenuItem {
            text: i18n("Open in Picture-in-Picture")
            icon.name: "view-zoom-out-symbolic"
            onTriggered: applicationWindow().openPiP(currentVideoId)
        }

        QQC2.MenuItem {
            text: videoMenu.isWatched ? i18n("Mark as unwatched") : i18n("Mark as watched")
            icon.name: videoMenu.isWatched ? "view-hidden" : "view-visible"
            onTriggered: {
                if (videoMenu.isWatched) {
                    videoModel.markAsUnwatched(currentVideoIndex);
                } else {
                    videoModel.markAsWatched(currentVideoIndex);
                }
            }
        }

        QQC2.MenuItem {
            text: i18n("Add to playlist")
            icon.name: "media-playlist-append"
            onTriggered: {
                addToPlaylistLoader.active = true;
                addToPlaylistLoader.item.open();
            }
        }

        ShareMenu {
            url: "https://youtube.com/watch?=" + currentVideoId
            shareTitle: currentVideoTitle
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
                            PlasmaTube.addToPlaylist(playlistId, currentVideoId);
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

