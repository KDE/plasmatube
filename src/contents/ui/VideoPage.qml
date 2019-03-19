/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.4 as Kirigami

import org.kde.plasmatube 1.0
import "utils.js" as Utils

Kirigami.ScrollablePage {
    id: root
    property string vid

    title: model.video.title
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    VideoModel {
        id: model
        videoId: vid
        onVideoIdChanged: {
            // is also executed in initial set
            model.fetch()
        }
    }

    ColumnLayout {
        MpvObject {
            id: renderer
            Layout.preferredWidth: root.width
            Layout.preferredHeight: root.width / 16.0 * 9.0
            Layout.maximumHeight: root.height

            MouseArea {
                anchors.fill: parent
                onClicked: renderer.command(["cycle", "pause"])
            }
        }

        // extra layout to make all details invisible while loading
        ColumnLayout {
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.fillWidth: true
            visible: !model.isLoading
            enabled: !model.isLoading

            // title
            Kirigami.Heading {
                Layout.fillWidth: true
                text: model.video.title
                wrapMode: Text.WordWrap
            }

            // author info and like statistics
            RowLayout {
                Layout.fillWidth: true
                //
                Image {
                    Layout.preferredHeight: 50
                    Layout.preferredWidth: 50
                    fillMode: Image.PreserveAspectFit
                    source: model.video.authorThumbnail(100)
                }

                Item {
                    Layout.fillWidth: true
                }

                ColumnLayout {
                    spacing: 0
                    Controls.ProgressBar {
                        Layout.preferredWidth: 150
                        value: model.video.rating / 5.0
                    }
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Kirigami.Icon {
                            source: "draw-arrow-up"
                            width: 22
                            height: 22
                        }
                        Controls.Label {
                            text: Utils.formatCount(model.video.likeCount)
                        }

                        // placeholder
                        Item {
                            width: 5
                        }

                        Kirigami.Icon {
                            source: "draw-arrow-down"
                            width: 22
                            height: 22
                        }
                        Controls.Label {
                            text: Utils.formatCount(model.video.dislikeCount)
                        }
                    }
                }
            }

            // video description
            Controls.Label {
                Layout.fillWidth: true
                text: model.video.description
                wrapMode: Text.WordWrap
            }
        }

        Controls.BusyIndicator {
            Layout.alignment: Qt.AlignCenter
            visible: model.isLoading
        }
    }

    Component.onCompleted: {
        renderer.command(["loadfile", "ytdl://" + vid])
    }
}
