// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasmatube 1.0
import org.kde.plasmatube.models 1.0
import "utils.js" as Utils

Kirigami.ScrollablePage {
    id: root

    title: i18n("Playlists")

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

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
        model: PlaylistsModel {
            id: videoModel
        }
        delegate: PlaylistGridItem {
            width: gridView.cellWidth
            height: gridView.cellHeight
        }
    }
}
