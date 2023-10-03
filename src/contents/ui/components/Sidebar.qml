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

    width: Kirigami.Units.gridUnit * 16
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

    function switchToPage(page) {
        if (applicationWindow().pageStack.currentItem !== page) {
            while (applicationWindow().pageStack.depth > 0) {
                applicationWindow().pageStack.pop();
            }
            applicationWindow().pageStack.push(page, 0);
        }
        applicationWindow().closePlayer();
    }

    contentItem: ColumnLayout {
        spacing: 0

        Kirigami.AbstractApplicationHeader {
            Layout.fillWidth: true
        }

        Delegates.RoundedItemDelegate {
            property var page: applicationWindow().getPage("PopularPage")
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "arrow-up-double"
            text: i18n("Popular")
            checked: pageStack.currentItem === page
            onClicked: {
                root.switchToPage(page);
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
                root.switchToPage(page);
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
                root.switchToPage(page);
            }
        }

        Delegates.RoundedItemDelegate {
            property var page: applicationWindow().getPage("SearchPage")
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "search"
            text: i18n("Search")
            checked: pageStack.currentItem === page
            onClicked: {
                root.switchToPage(page);
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
                root.switchToPage(page);
            }

            Layout.fillWidth: true
        }
    }
}

