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
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.8 as Kirigami

import org.kde.plasmatube.accountmanager 1.0

Kirigami.ApplicationWindow {
    id: root
    pageStack.initialPage: searchPageComponent
    globalDrawer: Kirigami.GlobalDrawer {
        title: "PlasmaTube"
        actions: [
            Kirigami.Action {
                text: qsTr("Log in")
                icon.name: "arrow-right"
                onTriggered: pageStack.layers.push(loginPageComponent)
                enabled: pageStack.layers.depth < 2
                visible: !AccountManager.username.length
            },
            Kirigami.Action {
                text: qsTr("Log out")
                icon.name: "system-log-out"
                visible: AccountManager.username.length > 0
                onTriggered: AccountManager.logOut()
            }
        ]
    }
    contextDrawer: Kirigami.ContextDrawer {}

    Component {
        id: searchPageComponent
        SearchPage {}
    }
    Component {
        id: videoPageComponent
        VideoPage {}
    }
    Component {
        id: loginPageComponent
        LoginPage {}
    }
}
