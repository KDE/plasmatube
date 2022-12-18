// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasmatube 1.0
import "videoplayer"
import "components"

Kirigami.ApplicationWindow {
    id: root
    pageStack.initialPage: getPage("TrendingPage")

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton

    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    readonly property real wideScreenThreshold: Kirigami.Units.gridUnit * 40
    readonly property bool isWidescreen: (root.width >= wideScreenThreshold) && root.wideScreen

    function getPage(name) {
        switch (name) {
            case "TrendingPage": return pagePool.loadPage("qrc:/TrendingPage.qml");
            case "SearchPage": return pagePool.loadPage("qrc:/SearchPage.qml");
            case "VideoPage": return pagePool.loadPage("qrc:/VideoPage.qml");
            case "SettingsPage": return pagePool.loadPage("qrc:/SettingsPage.qml");
        }
    }

    function switchVideo(video) {
        videoPlayer.switchVideo(video);
    }

    function openPlayer() {
        videoPlayer.open();
    }

    function closePlayer() {
        videoPlayer.close();
    }

    Loader {
        id: sidebarLoader
        source: "qrc:/components/Sidebar.qml"
        active: root.isWidescreen
    }

    Kirigami.PagePool {
        id: pagePool
    }

    globalDrawer: sidebarLoader.item
    contextDrawer: Kirigami.ContextDrawer {}

    footer: Loader {
        id: footerLoader
        visible: active
        active: !root.isWidescreen
        sourceComponent: BottomNavBar {
            shadow: !videoPlayer.isVideoLoaded
            opacity: (videoPlayer.contentY === 0) ? 1 : 0
            Behavior on opacity {
                NumberAnimation { duration: Kirigami.Units.shortDuration }
            }
        }
    }

    // FloatingVideoPreview {
    //     id: minimizedVideo
    //     z: 998
    //     parent: Controls.Overlay.overlay
    //     x: root.width - width - Kirigami.Units.gridUnit
    //     y: root.height - height - Kirigami.Units.gridUnit
    //     width: Kirigami.Units.gridUnit * 9
    //     height: Kirigami.Units.gridUnit * 6
    //     previewSource: videoPlayer.previewSource
    //
    //     onClicked: videoPlayer.open()
    // }

    // create space at the bottom to show miniplayer without it hiding stuff underneath
    pageStack.anchors.bottomMargin: videoPlayer.isVideoLoaded ? videoPlayer.miniPlayerHeight + 1 : 0

    VideoPlayerParent {
        id: videoPlayer
        anchors.fill: parent
        contentToPlayerSpacing: footerLoader.active ? footerLoader.height : 0
        onContentToPlayerSpacingChanged: console.log(footerLoader.active + ' ' + footerLoader.height)
        z: contentY === 0 ? -1 : 999
    }
}
