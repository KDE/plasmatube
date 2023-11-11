// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami

Kirigami.OverlayDrawer {
    id: root
    modal: false
    width: 100
    height: applicationWindow().height
    drawerOpen: Window.visibility !== Window.FullScreen

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

        QQC2.ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true

            QQC2.ScrollBar.vertical.policy: QQC2.ScrollBar.AlwaysOff
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
            contentWidth: -1 // disable horizontal scroll

            ColumnLayout {
                id: column
                width: scrollView.width
                spacing: 0

                Kirigami.NavigationTabButton {
                    property var page: applicationWindow().getPage("TrendingPage")
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                    icon.name: "videoclip-amarok"
                    text: i18n("Videos")
                    checked: pageStack.currentItem === page
                    onClicked: {
                        root.switchToPage(page);
                    }
                }

                Kirigami.NavigationTabButton {
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
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Layout.leftMargin: Kirigami.Units.smallSpacing
        }

        Kirigami.NavigationTabButton {
            property var page: applicationWindow().getPage("SettingsPage")
            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            icon.name: "settings-configure"
            text: i18n("Settings")
            checked: pageStack.currentItem === page
            onClicked: {
                root.switchToPage(page);
            }
        }
    }
}

