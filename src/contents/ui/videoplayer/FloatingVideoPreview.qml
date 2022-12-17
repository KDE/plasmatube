// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

import QtGraphicalEffects 1.15

import org.kde.kirigami 2.19 as Kirigami

MouseArea {
    id: root
    property alias previewSource: blur.source

    cursorShape: Qt.PointingHandCursor

    FastBlur {
        id: blur
        radius: 0
        anchors.fill: parent
    }
}
