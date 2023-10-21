// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

import org.kde.plasmatube
import "videoplayer"
import "components"

Kirigami.ApplicationWindow {
    id: root

    property bool hasSetInitialPage: false

    property Item hoverLinkIndicator: QQC2.Control {
        property string text

        parent: overlay.parent
        opacity: linkText.text.length > 0 ? 1 : 0

        x: 0
        y: parent.height - implicitHeight
        z: 999990

        contentItem: QQC2.Label {
            id: linkText
            text: parent.text
        }

        background: Rectangle {
             color: Kirigami.Theme.backgroundColor
        }

        Kirigami.Theme.colorSet: Kirigami.Theme.View
    }

    pageStack.initialPage: getPage("TrendingPage")

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton

    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    readonly property real wideScreenThreshold: Kirigami.Units.gridUnit * 40
    readonly property bool isWidescreen: (root.width >= wideScreenThreshold) && root.wideScreen

    function getPage(name) {
        switch (name) {
            case "PopularPage":
                return Qt.createComponent("org.kde.plasmatube", "PopularPage");
            case "SubscriptionsPage":
                return Qt.createComponent("org.kde.plasmatube", "SubscriptionsPage");
            case "TrendingPage":
                return Qt.createComponent("org.kde.plasmatube", "TrendingPage");
            case "SearchPage":
                return Qt.createComponent("org.kde.plasmatube", "SearchPage");
            case "VideoPage":
                return Qt.createComponent("org.kde.plasmatube", "VideoPage");
            case "SettingsPage":
                return Qt.createComponent("org.kde.plasmatube", "SettingsPage");
            case "HistoryPage":
                return Qt.createComponent("org.kde.plasmatube", "HistoryPage");
            case "PlaylistsPage":
                return Qt.createComponent("org.kde.plasmatube", "PlaylistsPage");
        }
    }

    function closePlayer() {
        videoPlayer.close();
    }

    globalDrawer: Sidebar {}

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
        z: contentY === 0 ? -1 : 999
    }

    Loader {
        id: pipLoader

        active: false

        sourceComponent: PictureInPictureVideo {
            onClosing: {
                PlasmaTube.videoController.videoMode = VideoController.Normal;
                pipLoader.active = false;
            }
        }

        onLoaded: item.vid = videoPlayer.currentVideoId
    }

    function switchToPage(page, args) {
        if (applicationWindow().pageStack.currentItem !== page) {
            while (applicationWindow().pageStack.depth > 0) {
                applicationWindow().pageStack.pop();
            }
            applicationWindow().pageStack.push(page, args);
        }
        applicationWindow().closePlayer();
    }

    Connections {
        target: PlasmaTube

        // Load the default homepage
        // TODO: hide the switch behind a loading screen ala NeoChat/Tokodon
        function onPreferencesChanged() {
            if (hasSetInitialPage) {
                return;
            }

            let defaultHome = PlasmaTube.preferences.defaultHome;
            switch (defaultHome) {
                case "Search":
                    root.switchToPage(getPage("SearchPage"));
                    break;
                case "Popular":
                    root.switchToPage(getPage("PopularPage"));
                    break;
                case "Trending":
                    root.switchToPage(getPage("TrendingPage"));
                    break;
                case "Subscriptions":
                    root.switchToPage(getPage("SubscriptionsPage"));
                    break;
                case "Playlists":
                    root.switchToPage(getPage("PlaylistsPage"));
                    break;
            }

            hasSetInitialPage = true;
        }
    }

    Connections {
        target: PlasmaTube.videoController

        function onOpenNormalPlayer() {
            PlasmaTube.videoController.currentPlayer = videoPlayer.previewSource;
            videoPlayer.open();
        }

        function onOpenPiPPlayer() {
            pipLoader.active = true;
            PlasmaTube.videoController.currentPlayer = pipLoader.item.renderer;

            // in our case we still want to open up the normal player anyway
            videoPlayer.open();
        }
    }
}
