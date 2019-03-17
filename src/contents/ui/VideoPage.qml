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
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.4 as Kirigami

import org.kde.plasmatube 1.0

Kirigami.Page {
    id: root

    property string url

    title: renderer.getProperty("media-title")
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    MpvObject {
        id: renderer
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onClicked: {
                renderer.command(["cycle", "pause"])
            }
        }
    }

    Component.onCompleted: {
        renderer.command(["loadfile", url])
        root.title = renderer.getProperty("media-title")
    }
}
