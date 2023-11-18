// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import Qt5Compat.GraphicalEffects

import org.kde.kirigami as Kirigami

QQC2.ItemDelegate {
    id: root
    signal contextMenuRequested

    leftPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing

    hoverEnabled: true
    onPressAndHold: contextMenuRequested()

    property real zoomScale: pressed ? 0.9 : 1
    Behavior on zoomScale {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutExpo
        }
    }

    transform: Scale {
        origin.x: root.width / 2;
        origin.y: root.height / 2;
        xScale: root.zoomScale
        yScale: root.zoomScale
    }

    background: Rectangle {
        anchors {
            fill: parent
            // TODO: switch to standard units
            topMargin: 1
            bottomMargin: 1
            leftMargin: 1
            rightMargin: 1
        }
        radius: Kirigami.Units.smallSpacing
        visible: root.hovered || root.activeFocus
        color: Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.hoverColor, root.hovered ? 0.1 : 0.2);
        border.color: Kirigami.Theme.hoverColor
        border.width: 1
    }
}