// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.delegates 1 as Delegates

import org.kde.plasmatube 1.0
import org.kde.plasmatube.models 1.0

Kirigami.OverlayDrawer {
    id: root

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
                    let page = applicationWindow().getPage("SearchPage");
                    applicationWindow().switchToPage(page);
                    page.doSearch(searchField.text);
                    searchField.text = "";
                }
            }
        }

        Delegates.RoundedItemDelegate {
            property var page: applicationWindow().getPage("PopularPage")
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "arrow-up-double"
            text: i18n("Popular")
            checked: pageStack.currentItem === page
            onClicked: {
                applicationWindow().switchToPage(page);
            }
        }

        Delegates.RoundedItemDelegate {
            property var page: applicationWindow().getPage("TrendingPage")
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "favorite"
            text: i18n("Trending")
            checked: pageStack.currentItem === page
            onClicked: {
                applicationWindow().switchToPage(page);
            }
        }

        Delegates.RoundedItemDelegate {
            property var page: applicationWindow().getPage("SubscriptionsPage")
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "videoclip-amarok"
            text: i18n("Subscriptions")
            checked: pageStack.currentItem === page
            enabled: PlasmaTube.isLoggedIn
            onClicked: {
                applicationWindow().switchToPage(page);
            }
        }

        Delegates.RoundedItemDelegate {
            property var page: applicationWindow().getPage("PlaylistsPage")
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "view-media-playlist"
            text: i18n("Playlists")
            checked: pageStack.currentItem === page
            enabled: PlasmaTube.isLoggedIn
            onClicked: {
                applicationWindow().switchToPage(page);
            }
        }

        Delegates.RoundedItemDelegate {
            property var page: applicationWindow().getPage("HistoryPage")
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "view-history"
            text: i18n("History")
            checked: pageStack.currentItem === page
            enabled: PlasmaTube.isLoggedIn
            onClicked: {
                applicationWindow().switchToPage(page);
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Delegates.RoundedItemDelegate {
            property var page: applicationWindow().getPage("SettingsPage")

            icon.name: "settings-configure"
            text: i18nc("@action:button Open settings dialog", "Settings")
            padding: Kirigami.Units.largeSpacing

            checked: pageStack.currentItem === page
            onClicked: {
                applicationWindow().switchToPage(page);
            }

            Layout.fillWidth: true
        }
    }
}

