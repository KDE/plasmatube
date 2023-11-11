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

QQC2.Control {
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
            id: root

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