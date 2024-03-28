// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

import org.kde.plasmatube

Kirigami.OverlayDrawer {
    id: root

    property alias trendingDelegate: trendingDelegate
    property alias subscriptionsDelegate: subscriptionsDelegate
    property alias playslistsDelegate: playslistsDelegate
    property alias historyDelegate: historyDelegate

    width: Kirigami.Units.gridUnit * 13
    height: applicationWindow().height
    edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    drawerOpen: !Kirigami.Settings.isMobile && enabled
    modal: !enabled || Kirigami.Settings.isMobile || Kirigami.Settings.tabletMode || (applicationWindow().width < Kirigami.Units.gridUnit * 50 && !collapsed) // Only modal when not collapsed, otherwise collapsed won't show.

    onModalChanged: drawerOpen = !modal

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    contentItem: ColumnLayout {
        id: column

        spacing: 0

        QQC2.ToolBar {
            Layout.fillWidth: true
            Layout.preferredHeight: pageStack.globalToolBar.preferredHeight
            Layout.bottomMargin: Kirigami.Units.smallSpacing / 2

            leftPadding: 3
            rightPadding: 3
            topPadding: 3
            bottomPadding: 3

            visible: !Kirigami.Settings.isMobile

            contentItem: Kirigami.SearchField {
                id: searchField

                autoAccept: false

                onAccepted: {
                    let page = Qt.createComponent("org.kde.plasmatube", "SearchPage");
                    applicationWindow().switchToPage(page, {initialSearch: searchField.text});
                    searchField.text = "";
                }
            }
        }

        SourceSwitcher {
            Layout.fillWidth: true
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        QQC2.ButtonGroup {
            id: pageButtonGroup
        }

        QQC2.ItemDelegate {
            id: trendingDelegate

            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "favorite"
            text: i18n("Trending")
            onClicked: {
                applicationWindow().switchToPage(Qt.createComponent("org.kde.plasmatube", "TrendingPage"));
                checked = true;
            }
            highlighted: checked

            QQC2.ButtonGroup.group: pageButtonGroup
        }

        QQC2.ItemDelegate {
            id: subscriptionsDelegate

            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "videoclip-amarok"
            text: i18n("Subscriptions")
            enabled: PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn
            onClicked: {
                applicationWindow().switchToPage(Qt.createComponent("org.kde.plasmatube", "SubscriptionsPage"));
                checked = true;
            }
            highlighted: checked

            QQC2.ButtonGroup.group: pageButtonGroup
        }

        QQC2.ItemDelegate {
            id: playslistsDelegate

            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "view-media-playlist"
            text: i18n("Playlists")
            enabled: PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn
            onClicked: {
                applicationWindow().switchToPage(Qt.createComponent("org.kde.plasmatube", "PlaylistsPage"));
                checked = true;
            }
            highlighted: checked

            QQC2.ButtonGroup.group: pageButtonGroup
        }

        QQC2.ItemDelegate {
            id: historyDelegate

            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "view-history"
            text: i18n("History")
            enabled: PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn
            onClicked: {
                applicationWindow().switchToPage(Qt.createComponent("org.kde.plasmatube", "HistoryPage"));
                checked = true;
            }
            highlighted: checked

            QQC2.ButtonGroup.group: pageButtonGroup
        }

        Item {
            Layout.fillHeight: true
        }

        QQC2.ItemDelegate {
            icon.name: "settings-configure"
            text: i18nc("@action:button Open settings dialog", "Settings")
            padding: Kirigami.Units.largeSpacing

            onClicked: {
                applicationWindow().pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "SettingsPage"), {}, {title: i18nc("@title:window", "Settings")});
            }

            Layout.fillWidth: true
        }
    }
}

