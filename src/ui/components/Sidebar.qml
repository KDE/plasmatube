// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.plasmatube

Kirigami.OverlayDrawer {
    id: root

    property alias trendingDelegate: trendingDelegate
    property alias subscriptionsDelegate: subscriptionsDelegate
    property alias playslistsDelegate: playslistsDelegate
    property alias historyDelegate: historyDelegate

    width: Kirigami.Units.gridUnit * 14
    edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    drawerOpen: !Kirigami.Settings.isMobile && enabled
    modal: !enabled || Kirigami.Settings.isMobile || Kirigami.Settings.tabletMode || (applicationWindow().width < Kirigami.Units.gridUnit * 50 && !collapsed) // Only modal when not collapsed, otherwise collapsed won't show.

    Behavior on width {
        NumberAnimation {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    onModalChanged: drawerOpen = !modal

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    component ActionDelegate: Delegates.RoundedItemDelegate {
        id: delegate

        padding: Kirigami.Units.largeSpacing
        Layout.fillWidth: true
        activeFocusOnTab: true

        onClicked: {
            if (delegate.checkable) {
                checked = true;
            }
            if (root.modal) {
                root.close();
            }
        }
    }

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
            Layout.topMargin: Math.round(Kirigami.Units.smallSpacing / 2)
            Layout.bottomMargin: 0
        }

        QQC2.ButtonGroup {
            id: pageButtonGroup
        }

        QQC2.ScrollView {
            id: scrollView

            contentWidth: availableWidth
            topPadding: Math.round(Kirigami.Units.smallSpacing / 2)
            bottomPadding: Math.round(Kirigami.Units.smallSpacing / 2)

            QQC2.ScrollBar.vertical.interactive: false

            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                spacing: 0

                width: scrollView.contentWidth
                height: Math.max(scrollView.availableHeight, implicitHeight)

                ActionDelegate {
                    id: trendingDelegate

                    icon.name: "favorite"
                    text: i18n("Trending")
                    checkable: true
                    onClicked: {
                        applicationWindow().switchToPage(Qt.createComponent("org.kde.plasmatube", "TrendingPage"));
                        checked = true;
                    }
                    QQC2.ButtonGroup.group: pageButtonGroup
                }

                ActionDelegate {
                    id: subscriptionsDelegate

                    Layout.fillWidth: true

                    icon.name: "rss-symbolic"
                    text: i18n("Subscriptions")
                    enabled: PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn
                    checkable: true
                    onClicked: {
                        applicationWindow().switchToPage(Qt.createComponent("org.kde.plasmatube", "SubscriptionsPage"));
                        checked = true;
                    }

                    QQC2.ButtonGroup.group: pageButtonGroup
                }

                ActionDelegate {
                    id: playslistsDelegate

                    icon.name: "view-media-playlist"
                    text: i18n("Playlists")
                    enabled: PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn
                    checkable: true
                    onClicked: {
                        applicationWindow().switchToPage(Qt.createComponent("org.kde.plasmatube", "PlaylistsPage"));
                        checked = true;
                    }

                    QQC2.ButtonGroup.group: pageButtonGroup
                }

                ActionDelegate {
                    id: historyDelegate

                    icon.name: "view-history"
                    text: i18n("History")
                    enabled: PlasmaTube.selectedSource !== null && PlasmaTube.selectedSource.loggedIn
                    checkable: true
                    onClicked: {
                        applicationWindow().switchToPage(Qt.createComponent("org.kde.plasmatube", "HistoryPage"));
                        checked = true;
                    }

                    QQC2.ButtonGroup.group: pageButtonGroup
                }

                Item {
                    Layout.fillHeight: true
                }

                ActionDelegate {
                    icon.name: "settings-configure"
                    text: i18nc("@action:button Open settings dialog", "Settings")

                    onClicked: {
                        applicationWindow().pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "SettingsPage"), {}, {title: i18nc("@title:window", "Settings")});
                    }
                }
            }
        }
    }
}

