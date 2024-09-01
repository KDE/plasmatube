// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.config as KConfig

import org.kde.plasmatube
import "videoplayer"
import "components"

Kirigami.ApplicationWindow {
    id: root

    KConfig.WindowStateSaver {
        configGroupName: "Window"
    }

    property Item hoverLinkIndicator: QQC2.Control {
        id: linkIndicator

        property alias text: linkText.text

        parent: root.overlay.parent
        opacity: linkText.text.length > 0 ? 1 : 0

        x: 0
        y: parent.height - implicitHeight
        z: 999990

        contentItem: QQC2.Label {
            id: linkText
        }

        background: Rectangle {
             color: Kirigami.Theme.backgroundColor
        }

        Kirigami.Theme.colorSet: Kirigami.Theme.View
    }

    title: {
        if (videoPlayerOpen && PlasmaTube.videoController.currentVideo) {
            return PlasmaTube.videoController.currentVideo.title;
        }

        return pageStack.currentItem?.title ?? "";
    }

    minimumWidth: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit * 22
    minimumHeight: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit * 20

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton

    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    readonly property real wideScreenThreshold: Kirigami.Units.gridUnit * 40
    readonly property bool isWidescreen: (root.width >= wideScreenThreshold) && root.wideScreen
    readonly property bool finishedLoading: PlasmaTube.sourceManager.hasAnySources ? PlasmaTube.sourceManager.finishedLoading : true
    readonly property bool videoPlayerOpen: videoPlayer.maximized

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

    Rectangle {
        anchors.fill: parent
        visible: !root.finishedLoading
        color: Kirigami.Theme.backgroundColor

        Kirigami.LoadingPlaceholder {
            anchors.centerIn: parent
        }
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
                root.loadDefaultPage();
            }
        }

        function onFinishedLoading() {
            root.loadDefaultPage();
        }
    }

    Component.onCompleted: {
        loadDefaultPage();
    }

    function loadDefaultPage() {
        if (!PlasmaTube.sourceManager.hasAnySources) {
            root.switchToPage(Qt.createComponent("org.kde.plasmatube", "WelcomePage"));
            return;
        }

        let defaultHome = "Trending";

        if (PlasmaTube.sourceManager.selectedSource !== null) {
            if (PlasmaTube.sourceManager.selectedSource.preferences.defaultHome.length !== 0) {
                defaultHome = PlasmaTube.sourceManager.selectedSource.preferences.defaultHome;
            }
        }

        switch (defaultHome) {
            case "Search":
                root.switchToPage(Qt.createComponent("org.kde.plasmatube", "SearchPage"));
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

    Loader {
        id: addToPlaylistLoader

        active: false
        sourceComponent: Kirigami.PromptDialog {
            id: addToPlaylistDialog

            property string currentVideoId

            title: i18nc("@title", "Add to Playlist")

            standardButtons: Kirigami.Dialog.NoButton

            mainItem: ColumnLayout {
                Repeater {
                    id: playlists

                    model: PlaylistsModel {
                        id: playlistsModel

                        Component.onCompleted: loadUserPlaylists()
                    }

                    delegate: QQC2.ItemDelegate {
                        required property var model

                        text: model.title

                        Layout.fillWidth: true

                        onClicked: {
                            PlasmaTube.selectedSource.addToPlaylist(model.id, addToPlaylistDialog.currentVideoId);
                            addToPlaylistDialog.close();
                        }
                    }
                }
            }
            onClosed: addToPlaylistLoader.active = false
        }
    }

    function openAddToPlaylistMenu(videoId: string): void {
        addToPlaylistLoader.active = true;
        addToPlaylistLoader.item.currentVideoId = videoId;
        addToPlaylistLoader.item.open();
    }
}
