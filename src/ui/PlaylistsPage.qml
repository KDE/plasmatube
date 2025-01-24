// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

import org.kde.kirigami as Kirigami

import org.kde.plasmatube

Kirigami.ScrollablePage {
    id: root

    title: i18n("Playlists")

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    BaseGridView {
        id: gridView

        placeholderIconName: "view-media-playlist"
        placeholderText: i18nc("@info:placeholder", "No Playlists")
        model: PlaylistsModel {
            id: playlistModel
        }
    }

    Component.onCompleted: playlistModel.loadUserPlaylists()
}
