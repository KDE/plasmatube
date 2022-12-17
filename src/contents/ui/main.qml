// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasmatube 1.0

Kirigami.ApplicationWindow {
    id: root
    pageStack.initialPage: getPage("TrendingPage")

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton

    readonly property real wideScreenThreshold: Kirigami.Units.gridUnit * 40
    readonly property bool isWidescreen: (root.width >= wideScreenThreshold) && root.wideScreen
    onIsWidescreenChanged: changeNav(isWidescreen);

    Component.onCompleted: {
        changeNav(isWidescreen);
    }
    
    // switch between bottom toolbar and sidebar
    function changeNav(toWidescreen) {
        if (toWidescreen) {
            if (footer !== null) {
                footer.destroy();
                footer = null;
            }
            sidebarLoader.active = true;
            globalDrawer = sidebarLoader.item;
        } else {
            sidebarLoader.active = false;
            globalDrawer = null;

            let bottomToolbar = Qt.createComponent("qrc:/components/BottomNavBar.qml")
            footer = bottomToolbar.createObject(root);
        }
    }

    function getPage(name) {
        switch (name) {
            case "TrendingPage": return pagePool.loadPage("qrc:/TrendingPage.qml");
            case "SearchPage": return pagePool.loadPage("qrc:/SearchPage.qml");
            case "VideoPage": return pagePool.loadPage("qrc:/VideoPage.qml");
            case "SettingsPage": return pagePool.loadPage("qrc:/SettingsPage.qml");
        }
    }

    Loader {
        id: sidebarLoader
        source: "qrc:/components/Sidebar.qml"
        active: false
    }

    Kirigami.PagePool {
        id: pagePool
    }

    contextDrawer: Kirigami.ContextDrawer {}
}
