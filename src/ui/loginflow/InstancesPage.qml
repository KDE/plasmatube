// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kitemmodels as KItemModels

import org.kde.plasmatube

Kirigami.ScrollablePage {
    id: root

    property alias model: listView.model
    property alias filterString: searchField.text
    
    header: QQC2.Control {
        padding: Kirigami.Units.largeSpacing

        background: Rectangle {
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            color: Kirigami.Theme.backgroundColor

            Kirigami.Separator {
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                    right: parent.right
                }
            }
        }

        contentItem: Kirigami.SearchField {
            id: searchField
            placeholderText: i18n("Search or enter URL…")
        }
    }

    ListView {
        id: listView

        header: QQC2.ItemDelegate {
            onClicked: root.addSource(root.filterString)

            activeFocusOnTab: false // We handle moving to this item via up/down arrows, otherwise the tab order is wacky

            contentItem: Kirigami.IconTitleSubtitle {
                icon.name: "compass-symbolic"
                title: root.filterString
                subtitle: i18n("Custom Server")
            }

            width: ListView.view.width
            height: visible ? implicitHeight : 0
            visible: !root.loading && listView.count === 0 && root.filterString !== ""
        }

        section.delegate: Kirigami.ListSectionHeader {
            width: ListView.view.width
            text: i18n("Public Servers")
        }
        section.property: "isPublic"

        delegate: QQC2.ItemDelegate {
            id: delegate

            required property int index
            required property string name
            required property string url
            required property string description
            required property string iconSource

            width: ListView.view.width

            contentItem: Kirigami.IconTitleSubtitle {
                icon.source: delegate.iconSource
                title: delegate.name
                subtitle: delegate.description
            }

            onClicked: root.addSource(delegate.url)
        }

        Kirigami.LoadingPlaceholder {
            anchors.centerIn: parent
            visible: root.loading
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No Public Servers")
            explanation: i18n("Enter a server URL manually in the search field.")
            visible: !root.loading && listView.count === 0 && root.filterString === ""
        }
    }
}
