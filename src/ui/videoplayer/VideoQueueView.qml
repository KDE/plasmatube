// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import Qt5Compat.GraphicalEffects

import org.kde.plasmatube
import "../components/utils.js" as Utils
import "../"
import "../components"

/*
 * This is the video queue on the right side of the video player.
 */
QQC2.Control {
    id: root

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    background: Rectangle {
        anchors.fill: parent
        color: Kirigami.Theme.backgroundColor
    }

    contentItem: ColumnLayout {
        spacing: 0

        QQC2.ToolBar {
            Layout.fillWidth: true

            QQC2.ToolButton {
                text: i18nc("@action:button Clear playlist", "Clear")
                onClicked: {
                    PlasmaTube.videoController.videoQueue.clear();
                    applicationWindow().closePlayer();
                }
            }
        }

        QQC2.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                model: PlasmaTube.videoController.videoQueue

                delegate: VideoListItem {
                    id: videoDelegate

                    width: ListView.view.width
                    height: implicitHeight

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
                    highlighted: model.playing

                    onPressed: PlasmaTube.videoController.videoQueue.playInQueue(model.index)
                }
            }
        }
    }
}