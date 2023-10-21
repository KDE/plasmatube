// SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschawn@kde.org>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.coreaddons as KCoreAddons
import org.kde.plasmatube

QQC2.Control {
    id: root

    readonly property var currentVideo: PlasmaTube.videoController.currentVideo
    readonly property var currentPlayer: PlasmaTube.videoController.currentPlayer

    property bool inFullScreen: false
    property bool showPresentationControls: true

    signal requestFullScreen()

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
                text: root.currentPlayer ? KCoreAddons.Format.formatDuration(root.currentPlayer.position * 1000) : ""
            }

            QQC2.Slider {
                Layout.fillWidth: true
                from: 0
                to: root.currentPlayer ? root.currentPlayer.duration : 0
                value: root.currentPlayer ? root.currentPlayer.position : 0

                onMoved: {
                    root.currentPlayer.setPosition(value)
                }
            }

            QQC2.Label {
                color: "white"
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                font.weight: Font.Bold
                text: root.currentPlayer ? KCoreAddons.Format.formatDuration(root.currentPlayer.duration * 1000) : ""
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.largeSpacing

            Item { Layout.fillWidth: true }

            RowLayout {
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


                RowLayout {

                    id: controlButtonBox
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.fillHeight: true
                    spacing: 2


                    QQC2.Button {
                        id: seekBackwardButton
                        implicitHeight: 40
                        implicitWidth: 40

                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false

                        onClicked: root.currentPlayer.seek(-5)
                        contentItem: Item{
                            Kirigami.Icon {
                                anchors.centerIn:parent
                                source:"media-seek-backward"
                                color: "white"
                                width: Kirigami.Units.gridUnit
                                height: Kirigami.Units.gridUnit

                            }
                        }
                        background: Kirigami.ShadowedRectangle{
                            corners.topLeftRadius: 7
                            corners.bottomLeftRadius: 7


                            color: if (parent.down){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                                }else if(parent.hovered){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                }else{
                                    Qt.rgba(1, 1, 1, 0.3)
                                }
                        }
                        TabIndicator {}
                    }

                    QQC2.Button {
                        id: playPauseButton
                        implicitHeight: 40
                        implicitWidth: 60
                        onClicked: {
                            if (root.currentPlayer.paused) {
                                root.currentPlayer.play();
                            } else {
                                root.currentPlayer.pause();
                            }
                        }
                        contentItem: Item {
                            Kirigami.Icon {
                                anchors.centerIn:parent
                                source: root.currentPlayer?.paused ? "media-playback-start" : "media-playback-pause"
                                color: "white"
                                width: Kirigami.Units.gridUnit
                                height: Kirigami.Units.gridUnit
                            }
                        }
                        background: Kirigami.ShadowedRectangle{
                            color: if (parent.down){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                                }else if(parent.hovered){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                }else{
                                    Qt.rgba(1, 1, 1, 0.3)
                                }
                        }
                        TabIndicator {}
                    }

                    QQC2.Button {
                        id: seekForwardButton
                        implicitHeight: 40
                        implicitWidth: 40
                        Layout.rightMargin:isWidescreen?0:10

                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false

                        onClicked: root.currentPlayer.seek(5)

                        contentItem: Item{
                            Kirigami.Icon {
                                anchors.centerIn:parent
                                source:"media-seek-forward"
                                color: "white"
                                width: Kirigami.Units.gridUnit
                                height: Kirigami.Units.gridUnit

                            }
                        }
                        background: Kirigami.ShadowedRectangle{
                            corners.topRightRadius: 7
                            corners.bottomRightRadius: 7
                            color: if (parent.down){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                                }else if(parent.hovered){
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                }else{
                                    Qt.rgba(1, 1, 1, 0.3)
                                }
                        }
                        TabIndicator {}
                    }
                }

                QQC2.ToolButton {
                    width: Kirigami.Units.gridUnit * 3
                    height: width
                    icon.name: "view-fullscreen"
                    icon.color: "white"
                    icon.width: Kirigami.Units.iconSizes.smallMedium
                    icon.height: Kirigami.Units.iconSizes.smallMedium

                    visible: root.showPresentationControls

                    onClicked: {
                        root.requestFullScreen();
                    }

                    TabIndicator {}
                }

                QQC2.ToolButton {
                    width: Kirigami.Units.gridUnit * 3
                    height: width
                    icon.name: "view-zoom-in-symbolic"
                    icon.color: "white"
                    icon.width: Kirigami.Units.iconSizes.smallMedium
                    icon.height: Kirigami.Units.iconSizes.smallMedium

                    visible: root.showPresentationControls

                    onClicked: PlasmaTube.videoController.videoMode = VideoController.PictureInPicture

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
                model: PlasmaTube.videoController.videoModel.formatList
                delegate: QQC2.RadioDelegate {
                    Layout.fillWidth: true
                    checked: PlasmaTube.videoController.videoModel.selectedFormat === modelData
                    text: modelData
                    QQC2.ButtonGroup.group: radioGroup
                    onCheckedChanged: {
                        if (checked) {
                            PlasmaTube.videoController.videoModel.selectedFormat = modelData
                        }
                    }
                }
            }
        }
    }
}
