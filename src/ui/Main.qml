// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

import org.kde.plasmatube
import "videoplayer"
import "components"

Kirigami.ApplicationWindow {
    id: root

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

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton

    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    readonly property real wideScreenThreshold: Kirigami.Units.gridUnit * 40
    readonly property bool isWidescreen: (root.width >= wideScreenThreshold) && root.wideScreen
    readonly property bool finishedLoading: PlasmaTube.sourceManager.hasAnySources ? PlasmaTube.sourceManager.finishedLoading : true

    function closePlayer() {
        videoPlayer.close();
    }

    globalDrawer: Sidebar {
        id: sidebar
        enabled: PlasmaTube.sourceManager.hasAnySources && root.finishedLoading
    }

    footer: Loader {
        id: footerLoader
        visible: active
        active: !root.isWidescreen && PlasmaTube.sourceManager.hasAnySources
        sourceComponent: BottomNavBar {
            opacity: (videoPlayer.contentY === 0) ? 1 : 0

            Behavior on opacity {
                NumberAnimation { duration: Kirigami.Units.shortDuration }
            }
        }
    }

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
    }

    Kirigami.PlaceholderMessage {
        anchors.fill: parent
        text: i18nc("@info:status", "Loading…")
        visible: !root.finishedLoading
    }

    function switchToPage(page, args) {
        applicationWindow().pageStack.layers.clear();
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

        function onOpenVideo(videoId: string) {
            PlasmaTube.videoController.play(videoId);
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

    Connections {
        target: PlasmaTube.sourceManager

        function onSourceSelected() {
            if (PlasmaTube.sourceManager.finishedLoading) {
                loadDefaultPage();
            }
        }

        function onFinishedLoading() {
            loadDefaultPage();
        }
    }

    Component.onCompleted: {
        loadDefaultPage();
        saveWindowGeometryConnections.enabled = true;
    }

    function loadDefaultPage() {
        if (!PlasmaTube.sourceManager.hasAnySources) {
            root.switchToPage(Qt.createComponent("org.kde.plasmatube", "WelcomePage"));
            return;
        }

        let defaultHome = "Popular";

        if (PlasmaTube.sourceManager.selectedSource !== null) {
            if (PlasmaTube.sourceManager.selectedSource.preferences.defaultHome.length !== 0) {
                defaultHome = PlasmaTube.sourceManager.selectedSource.preferences.defaultHome;
            } else if(!PlasmaTube.sourceManager.selectedSource.supportsPopularPage()) {
                defaultHome = "Trending";
            }
        }

        switch (defaultHome) {
            case "Search":
                root.switchToPage(Qt.createComponent("org.kde.plasmatube", "SearchPage"));
                sidebar.searchDelegate.checked = true;
                break;
            case "Popular":
                root.switchToPage(Qt.createComponent("org.kde.plasmatube", "PopularPage"));
                sidebar.popularDelegate.checked = true;
                break;
            case "Trending":
                root.switchToPage(Qt.createComponent("org.kde.plasmatube", "TrendingPage"));
                sidebar.trendingDelegate.checked = true;
                break;
            case "Subscriptions":
                root.switchToPage(Qt.createComponent("org.kde.plasmatube", "SubscriptionsPage"));
                sidebar.subscriptionsDelegate.checked = true;
                break;
            case "Playlists":
                root.switchToPage(Qt.createComponent("org.kde.plasmatube", "PlaylistsPage"));
                sidebar.playslistsDelegate.checked = true;
                break;
        }
    }

    // This timer allows to batch update the window size change to reduce
    // the io load and also work around the fact that x/y/width/height are
    // changed when loading the page and overwrite the saved geometry from
    // the previous session.
    Timer {
        id: saveWindowGeometryTimer
        interval: 1000
        onTriggered: WindowController.saveGeometry()
    }

    Connections {
        id: saveWindowGeometryConnections
        enabled: false // Disable on startup to avoid writing wrong values if the window is hidden
        target: root

        function onClosing() { WindowController.saveGeometry(); }
        function onWidthChanged() { saveWindowGeometryTimer.restart(); }
        function onHeightChanged() { saveWindowGeometryTimer.restart(); }
        function onXChanged() { saveWindowGeometryTimer.restart(); }
        function onYChanged() { saveWindowGeometryTimer.restart(); }
    }
}
