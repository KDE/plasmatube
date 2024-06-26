// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

import org.kde.plasmatube

/*
 * This is the video queue on the right side of the video player.
 */
QQC2.Control {
    id: root

    topPadding: 0
    bottomPadding: Kirigami.Units.mediumSpacing
    leftPadding: 0
    rightPadding: 0

    background: Rectangle {
        anchors.fill: parent
        color: Kirigami.Theme.backgroundColor
        radius: Kirigami.Units.mediumSpacing

        Kirigami.Theme.colorSet: Kirigami.Theme.Header
        Kirigami.Theme.inherit: false
    }

    contentItem: ColumnLayout {
        spacing: 0

        Kirigami.Heading {
            text: i18n("Queue")

            Layout.margins: Kirigami.Units.largeSpacing
        }

        QQC2.ToolBar {
            Layout.fillWidth: true

            QQC2.ToolButton {
                text: i18nc("@action:button Clear playlist", "Clear")
                icon.name: "edit-clear-all"
                onClicked: {
                    PlasmaTube.videoController.videoQueue.clear();
                    applicationWindow().closePlayer();
                }
            }
        }

        QQC2.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true

            background: Rectangle {
                color: Kirigami.Theme.alternateBackgroundColor

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false
            }

            ListView {
                model: PlasmaTube.videoController.videoQueue

                delegate: VideoListItem {
                    id: videoDelegate

                    required property var model

                    width: ListView.view.width
                    height: implicitHeight

                    vid: model?.id ?? ""
                    thumbnail: model?.thumbnail ?? ""
                    liveNow: model?.liveNow ?? false
                    length: model?.length ?? 0
                    title: model?.title ?? i18n("Loading…")
                    author: model?.author ?? ""
                    authorId: model?.authorId ??  ""
                    description: model?.description ?? ""
                    viewCount: model?.viewCount ?? 0
                    publishedText: model?.publishedText ?? ""
                    watched: model?.watched ?? false
                    highlighted: model?.playing ?? false

                    onClicked: PlasmaTube.videoController.videoQueue.playInQueue(model.index)
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }
    }
}