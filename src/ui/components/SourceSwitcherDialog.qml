// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.labs.components as Components

import org.kde.plasmatube

Kirigami.Dialog {
    id: root

    parent: applicationWindow().overlay

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    standardButtons: Kirigami.Dialog.NoButton

    width: Math.min(applicationWindow().width, Kirigami.Units.gridUnit * 24)
    title: i18n("Switch Source")

    onVisibleChanged: if (visible) {
        sources.forceActiveFocus()
    }

    contentItem: ListView {
        id: sources

        implicitHeight: contentHeight

        model: PlasmaTube.sourceManager
        currentIndex: PlasmaTube.sourceManager.selectedIndex

        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false

        footer: QQC2.ItemDelegate {
            id: addSourceDelegate

            width: parent.width
            highlighted: focus
            icon {
                name: "list-add"
                width: Kirigami.Units.iconSizes.medium
                height: Kirigami.Units.iconSizes.medium
            }
            text: i18n("Add Source")

            contentItem: Delegates.SubtitleContentItem {
                itemDelegate: addSourceDelegate
                subtitle: i18n("Add a new video source")
            }

            onClicked: {
                pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "WelcomePage"));
                root.close();
            }

            Keys.onUpPressed: {
                sources.currentIndex = sources.count - 1;
                sources.forceActiveFocus();
            }
            Keys.onDownPressed: {
                sources.currentIndex = 0;
                sources.forceActiveFocus();
            }
            Keys.onEnterPressed: clicked()
            Keys.onReturnPressed: clicked()
        }

        clip: true

        keyNavigationEnabled: false
        Keys.onDownPressed: {
            if (sources.currentIndex === sources.count - 1) {
                footerItem.forceActiveFocus();
                sources.currentIndex = -1;
            } else {
                sources.incrementCurrentIndex();
            }
        }
        Keys.onUpPressed: {
            if (sources.currentIndex === 0) {
                footerItem.forceActiveFocus();
                sources.currentIndex = -1;
            } else {
                sources.decrementCurrentIndex();
            }
        }
        Keys.onEnterPressed: currentItem?.clicked()
        Keys.onReturnPressed: currentItem?.clicked()

        Layout.fillWidth: true
        Layout.preferredHeight: contentHeight
        Layout.topMargin: Kirigami.Units.smallSpacing

        delegate: QQC2.ItemDelegate {
            id: sourceDelegate

            required property int index
            required property var source

            text: source.url
            width: ListView.view.width

            onClicked: {
                if (PlasmaTube.sourceManager.selectedSource !== sourceDelegate.source) {
                    PlasmaTube.sourceManager.selectedSource = sourceDelegate.source;
                    sources.currentIndex = sourceDelegate.index;
                    PlasmaTube.saveCurrentSource();
                }
                root.close();
            }
        }
    }
}
