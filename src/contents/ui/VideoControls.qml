// SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschawn@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10 as QQC2
import QtMultimedia 5.12 as Multimedia
import org.kde.kirigami 2.10 as Kirigami
import org.kde.kcoreaddons 1.0 as KCoreAddons

QQC2.Control {
    required property var video
    required property var audio
    anchors.fill: parent
    Rectangle {
        height: parent.children[1].height * 3
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(18/255, 18/255, 18/255, 0.0) }
            GradientStop { position: 1.0; color: Qt.rgba(18/255, 18/255, 18/255, 0.8) }
        }
    }
    ColumnLayout {
        visible: true //Hologram.Video.mediaObject.error == Multimedia.MediaPlayer.NoError
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            left: parent.left
            right: parent.right
            leftMargin: Kirigami.Units.gridUnit
            rightMargin: Kirigami.Units.gridUnit
        }

        Kirigami.Theme.textColor: Qt.rgba(1,1,1,0.8)
        RowLayout {
            Layout.fillWidth: true
            Item {Layout.fillWidth: true}
            Kirigami.Heading {
                level: 4
                color: "white"
                text: KCoreAddons.Format.formatDuration(video.position)
            }
            QQC2.Slider {
                visible: video.seekable
                Layout.fillWidth: true
                from: 0
                to: video.duration
                value: video.position / video.duration
                onMoved: {
                    console.log( video.seekable)
                    video.seek(value * video.duration)
                    audio.seek(value * video.duration)
                    console.log(value, video.duration, value * video.duration);
                    /*value = Qt.binding(function() {
                        return video.position / video.duration;
                    });*/
                }
            }
            Kirigami.Heading {
                level: 4
                color: "white"
                text: "-" + KCoreAddons.Format.formatDuration(video.duration)
            }
            Item {Layout.fillWidth: true}
        }
        RowLayout {
            Layout.fillWidth: true
            Item {Layout.fillWidth: true}
            Row {
                Layout.alignment: Qt.AlignHCenter
                spacing: Kirigami.Units.largeSpacing
                QQC2.ToolButton {
                    width: Kirigami.Units.iconSizes.large
                    height: width
                    icon.name: "media-skip-backward"
                    icon.color: "white"
                    //enabled: Hologram.Video.playlist.hasPrevious
                    //onClicked: Hologram.Video.playlist.previous()

                    TabIndicator {}
                }
                QQC2.ToolButton {
                    width: Kirigami.Units.iconSizes.large
                    height: width
                    icon.name: "media-seek-backward"
                    icon.color: "white"

                    TabIndicator {}

                    onClicked: {
                        const value = video.position - 5000 < 0 ? 0 : video.position - 5000;
                        video.seek(value)
                        audio.seek(value)
                    }
                }
                QQC2.ToolButton {
                    width: Kirigami.Units.iconSizes.large
                    height: width

                    icon.name: video.playbackState !== Multimedia.MediaPlayer.PlayingState ? "media-playback-start" : "media-playback-pause"
                    icon.color: "white"

                    TabIndicator {}

                    onClicked: {
                        if (video.playbackState !== Multimedia.MediaPlayer.PlayingState) {
                            video.play();
                        } else {
                            video.pause();
                        }
                    }
                }
                QQC2.ToolButton {
                    width: Kirigami.Units.iconSizes.large
                    height: width
                    icon.name: "media-seek-forward"
                    icon.color: "white"

                    TabIndicator {}

                    onClicked: {
                        const value = video.position + 5000 > video.duration ? video.duration : video.position + 5000;
                        video.seek(value)
                        audio.seek(value)
                    }
                }
                QQC2.ToolButton {
                    width: Kirigami.Units.iconSizes.large
                    height: width
                    icon.name: "media-skip-forward"
                    icon.color: "white"

                    //enabled: Hologram.Video.playlist.hasNext
                    //onlicked: Hologram.Video.playlist.next()

                    TabIndicator {}
                }
            }
            Item {Layout.fillWidth: true}
        }
        Item { height: Kirigami.Units.largeSpacing }
    }
}
