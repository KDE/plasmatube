// SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschawn@kde.org>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.10
import QtQuick.Layouts 1.10
import QtQuick.Window 2.15
import QtQuick.Controls 2.10 as QQC2

import QtMultimedia 5.12 as Multimedia

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kcoreaddons 1.0 as KCoreAddons

QQC2.Control {
    required property var video

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
        visible: true
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            leftMargin: Kirigami.Units.largeSpacing
            rightMargin: Kirigami.Units.largeSpacing
        }

        Kirigami.Theme.textColor: Qt.rgba(1,1,1,0.8)

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
            QQC2.Label {
                color: "white"
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                font.weight: Font.Bold
                text: KCoreAddons.Format.formatDuration(video.position * 1000)
            }

            QQC2.Slider {
                Layout.fillWidth: true
                from: 0
                to: video.duration
                value: video.position

                onMoved: {
                    video.setPosition(value)
                }
            }

            QQC2.Label {
                color: "white"
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                font.weight: Font.Bold
                text: KCoreAddons.Format.formatDuration(video.duration * 1000)
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.largeSpacing

            Item { Layout.fillWidth: true }

            Row {
                Layout.alignment: Qt.AlignHCenter
                spacing: Kirigami.Units.largeSpacing

                QQC2.ToolButton {
                    width: Kirigami.Units.gridUnit * 3
                    height: width
                    icon.name: "configure"
                    icon.color: "white"
                    icon.width: Kirigami.Units.iconSizes.smallMedium
                    icon.height: Kirigami.Units.iconSizes.smallMedium

                    onClicked: {
                        configureDialog.open()
                    }

                    TabIndicator {}
                }

                QQC2.ToolButton {
                    width: Kirigami.Units.gridUnit * 3
                    height: width
                    icon.name: "media-seek-backward"
                    icon.color: "white"
                    icon.width: Kirigami.Units.iconSizes.smallMedium
                    icon.height: Kirigami.Units.iconSizes.smallMedium

                    TabIndicator {}

                    onClicked: {
                        video.seek(-5);
                    }
                }

                QQC2.ToolButton {
                    width: Kirigami.Units.gridUnit * 3
                    height: width

                    icon.name: video.paused ? "media-playback-start" : "media-playback-pause"
                    icon.color: "white"
                    icon.width: Kirigami.Units.iconSizes.smallMedium
                    icon.height: Kirigami.Units.iconSizes.smallMedium

                    TabIndicator {}

                    onClicked: {
                        if (video.paused) {
                            video.play();
                        } else {
                            video.pause();
                        }
                    }
                }

                QQC2.ToolButton {
                    width: Kirigami.Units.gridUnit * 3
                    height: width
                    icon.name: "media-seek-forward"
                    icon.color: "white"
                    icon.width: Kirigami.Units.iconSizes.smallMedium
                    icon.height: Kirigami.Units.iconSizes.smallMedium

                    TabIndicator {}

                    onClicked: {
                        video.seek(5);
                    }
                }

                QQC2.ToolButton {
                    width: Kirigami.Units.gridUnit * 3
                    height: width
                    icon.name: "view-fullscreen"
                    icon.color: "white"
                    icon.width: Kirigami.Units.iconSizes.smallMedium
                    icon.height: Kirigami.Units.iconSizes.smallMedium

                    onClicked: {
                        if (applicationWindow().visibility !== Window.FullScreen) {
                            applicationWindow().showFullScreen();
                            applicationWindow().openPlayer();
                        } else {
                            applicationWindow().showNormal();
                        }
                    }

                    TabIndicator {}
                }
            }

            Item { Layout.fillWidth: true }
        }
    }

    Kirigami.Dialog {
        id: configureDialog
        title: i18n("Settings")
        padding: 0
        preferredHeight: Kirigami.Units.gridUnit * 20
        preferredWidth: Kirigami.Units.gridUnit * 15

        ColumnLayout {
            spacing: 0
            QQC2.ButtonGroup {
                id: radioGroup
            }
            Repeater {
                model: video.video.formatList
                delegate: QQC2.RadioDelegate {
                    Layout.fillWidth: true
                    checked: video.video.selectedFormat === modelData
                    text: modelData
                    QQC2.ButtonGroup.group: radioGroup
                    onCheckedChanged: {
                        if (checked) {
                            video.video.selectedFormat = modelData
                        }
                    }
                }
            }
        }
    }
}
