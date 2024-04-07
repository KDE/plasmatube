// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.plasmatube

QQC2.Menu {
    id: root

    property bool isWatched
    required property string videoId
    required property string videoUrl
    required property string videoTitle
    required property string channelName
    required property string channelId
    property bool shouldRemoveFromPlaylist

    // Queue controls only make sense if you have a video loaded
    readonly property bool shouldShowQueueControls: PlasmaTube.videoController.currentVideo.isLoaded

    signal markWatched()
    signal markUnwatched()
    signal addToPlaylist()
    signal removeFromPlaylist()

    modal: true

    QQC2.MenuItem {
        text: root.shouldShowQueueControls ? i18n("Play Now") : i18n("Play")
        icon.name: "media-playback-start"
        onTriggered: {
            root.markWatched();
            PlasmaTube.videoController.videoMode = VideoController.Normal;
            PlasmaTube.videoController.play(root.videoId);
        }
    }

    QQC2.MenuItem {
        text: i18n("Play Next")
        icon.name: "media-playlist-append"
        visible: root.shouldShowQueueControls
        onTriggered: {
            PlasmaTube.videoController.queueNext(root.videoId);
        }
    }

    QQC2.MenuItem {
        text: i18n("Play in Picture-in-Picture")
        icon.name: "window-duplicate-symbolic"
        onTriggered: {
            root.markWatched();
            PlasmaTube.videoController.videoMode = VideoController.PictureInPicture;
            PlasmaTube.videoController.play(root.videoId);
        }
    }

    QQC2.MenuSeparator {
        visible: PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn
    }

    QQC2.MenuItem {
        text: root.isWatched ? i18n("Mark as Unwatched") : i18n("Mark as Watched")
        icon.name: root.isWatched ? "view-hidden" : "view-visible"
        visible: PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn
        onTriggered: {
            if (root.isWatched) {
                root.markUnwatched();
            } else {
                root.markWatched();
            }
            root.isWatched = !root.isWatched;
        }
    }

    QQC2.MenuItem {
        text: i18n("Add to Playlist…")
        icon.name: "media-playlist-append"
        onTriggered: root.addToPlaylist()
        visible: !root.shouldRemoveFromPlaylist && (PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn)
    }

    QQC2.MenuItem {
        text: i18n("Remove from Playlist")
        icon.name: "remove"
        onTriggered: root.removeFromPlaylist()
        visible: root.shouldRemoveFromPlaylist && (PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn)
    }

    QQC2.MenuSeparator {}

    QQC2.MenuItem {
        text: root.channelName
        icon.name: "user"
        onTriggered: pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "ChannelPage"), {author: root.channelName, authorId: root.channelId});
    }

    QQC2.MenuSeparator {}

    ShareMenu {
        url: root.videoUrl
        shareTitle: root.videoTitle
    }
}