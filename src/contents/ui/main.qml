// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.8 as Kirigami

import org.kde.plasmatube 1.0

Kirigami.ApplicationWindow {
    id: root
    pageStack.initialPage: searchPageComponent
    globalDrawer: Kirigami.GlobalDrawer {
        title: "PlasmaTube"
        actions: [
            Kirigami.Action {
                text: PlasmaTube.isLoggedIn ? qsTr("Log out") : qsTr("Log in")
                icon.name: PlasmaTube.isLoggedIn ? "system-log-out" : "arrow-right"
                onTriggered: {
                    if (PlasmaTube.isLoggedIn) {
                        PlasmaTube.logOut()
                    } else {
                        pageStack.layers.push(loginPageComponent)
                    }
                }
                enabled: pageStack.layers.depth < 2
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
