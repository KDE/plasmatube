// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtGraphicalEffects

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
