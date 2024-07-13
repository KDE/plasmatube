// SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschawn@kde.org>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.coreaddons as KCoreAddons
import org.kde.plasmatube

/*
 * These are the video controls
 */
QQC2.Control {
    id: root

    readonly property var currentVideo: PlasmaTube.videoController.currentVideo
    readonly property var currentPlayer: PlasmaTube.videoController.currentPlayer

    // TODO: move completely to C++
    readonly property bool videoLoaded: currentPlayer !== null && currentPlayer.duration > 0
    readonly property bool durationMatchesPosition: currentPlayer !== null && Math.abs(currentPlayer.duration - currentPlayer.position) < 0.1
    readonly property bool atEnd: currentPlayer !== null && currentPlayer.paused && videoLoaded && durationMatchesPosition

    property bool inFullScreen: false
    property bool showPresentationControls: true

    signal requestFullScreen
    signal sliderMoved

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
                    root.currentPlayer.position = value;
                    root.sliderMoved();
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
                    text: i18nc("@action:button", "Video settings")
                    icon.name: "configure"
                    icon.color: "white"
                    icon.width: Kirigami.Units.iconSizes.smallMedium
                    icon.height: Kirigami.Units.iconSizes.smallMedium
                    display: QQC2.AbstractButton.IconOnly

                    onClicked: {
                        configureDialog.open()
                    }

                    TabIndicator {}

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
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
                        text: i18nc("@action:button", "Seek backward")

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

                            color: {
                                if (seekBackwardButton. down) {
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                                } else if (parent.hovered) {
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                } else {
                                    Qt.rgba(1, 1, 1, 0.3)
                                }
                            }
                        }
                        TabIndicator {}

                        QQC2.ToolTip.text: text
                        QQC2.ToolTip.visible: hovered
                        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    }

                    QQC2.Button {
                        id: playPauseButton
                        implicitHeight: 40
                        implicitWidth: 60
                        onClicked: PlasmaTube.videoController.togglePlaying()
                        text: i18nc("@action:button", "Play")
                        contentItem: Item {
                            Kirigami.Icon {
                                anchors.centerIn:parent
                                source: {
                                    if (root.currentPlayer?.paused) {
                                        if (root.atEnd) {
                                            return "media-playlist-repeat";
                                        } else {
                                            return "media-playback-start";
                                        }
                                    } else {
                                        return "media-playback-pause";
                                    }
                                }
                                color: "white"
                                width: Kirigami.Units.gridUnit
                                height: Kirigami.Units.gridUnit
                            }
                        }
                        background: Kirigami.ShadowedRectangle{
                            color: {
                                if (playPauseButton.down) {
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                                } else if (parent.hovered) {
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                } else {
                                    Qt.rgba(1, 1, 1, 0.3)
                                }
                            }
                        }
                        TabIndicator {}

                        QQC2.ToolTip.text: text
                        QQC2.ToolTip.visible: hovered
                        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    }

                    QQC2.Button {
                        id: seekForwardButton

                        implicitHeight: 40
                        implicitWidth: 40

                        text: i18nc("@action:button", "Seek forward")

                        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                        Kirigami.Theme.inherit: false

                        onClicked: root.currentPlayer.seek(5)

                        contentItem: Item {
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
                            color: {
                                if (seekForwardButton.down) {
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.3)
                                } else if (parent.hovered) {
                                    Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.hoverColor, "transparent", 0.7)
                                } else {
                                    Qt.rgba(1, 1, 1, 0.3)
                                }
                            }
                        }
                        TabIndicator {}

                        QQC2.ToolTip.text: text
                        QQC2.ToolTip.visible: hovered
                        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    }
                }

                QQC2.ToolButton {
                    implicitWidth: 40
                    implicitHeight: implicitWidth
                    icon.name: "view-fullscreen"
                    icon.color: "white"
                    icon.width: Kirigami.Units.iconSizes.smallMedium
                    icon.height: Kirigami.Units.iconSizes.smallMedium
                    text: i18nc("@action:button", "Fullscreen")
                    display: QQC2.AbstractButton.IconOnly

                    visible: root.showPresentationControls

                    onClicked: {
                        root.requestFullScreen();
                    }

                    TabIndicator {}

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                }

                QQC2.ToolButton {
                    implicitWidth: 40
                    implicitHeight: implicitWidth
                    icon.name: "window-duplicate-symbolic"
                    icon.color: "white"
                    icon.width: Kirigami.Units.iconSizes.smallMedium
                    icon.height: Kirigami.Units.iconSizes.smallMedium
                    text: i18nc("@action:button", "Picture-in-picture")
                    display: QQC2.AbstractButton.IconOnly

                    visible: root.showPresentationControls && !Kirigami.Settings.isMobile

                    onClicked: PlasmaTube.videoController.videoMode = VideoController.PictureInPicture

                    TabIndicator {}

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
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
                    id: delegate

                    required property var modelData

                    Layout.fillWidth: true
                    checked: PlasmaTube.videoController.videoModel.selectedFormat === modelData
                    text: modelData
                    QQC2.ButtonGroup.group: radioGroup
                    onCheckedChanged: {
                        if (checked) {
                            PlasmaTube.videoController.videoModel.selectedFormat = delegate.modelData;
                        }
                    }
                }
            }
        }
    }
}
