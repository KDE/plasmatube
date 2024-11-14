// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

import org.kde.kirigami as Kirigami

import org.kde.plasmatube

Kirigami.ScrollablePage {
    required property string playlistId

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

    header: Kirigami.InlineMessage {
        id: message
        type: Kirigami.MessageType.Error
        width: parent.width
        position: InlineMessage.Position.Inline

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

    BaseGridView {
        id: gridView

        playlistId: root.playlistId

        model: VideoListModel {
            id: videoModel
        }
    }

    Component.onCompleted: videoModel.requestPlaylist(playlistId)
}
