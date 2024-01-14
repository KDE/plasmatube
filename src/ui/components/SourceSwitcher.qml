// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.labs.components as KirigamiComponents

import org.kde.plasmatube

QQC2.Pane {
    id: root

    readonly property var selectedSource: PlasmaTube.sourceManager.selectedSource

    property alias sourcesListVisible: sources.visible

    padding: 0

    contentItem: ColumnLayout {
        id: content

        spacing: 0

        QQC2.ItemDelegate {
            id: currentSourceDelegate

            text: root.selectedSource?.url

            Layout.fillWidth: true

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Delegates.SubtitleContentItem {
                    itemDelegate: currentSourceDelegate
                    subtitle: {
                        switch (root.selectedSource?.type) {
                            case VideoSource.Invidious:
                                return i18n("Invidious");
                            case VideoSource.PeerTube:
                                return i18n("PeerTube");
                            case VideoSource.Piped:
                                return i18n("Piped");
                        }
                    }
                    subtitleItem.textFormat: Text.PlainText

                    Layout.fillWidth: true
                }

                QQC2.ToolButton {
                    text: i18n("Switch Source")
                    icon.name: "system-switch-user"

                    display: QQC2.AbstractButton.IconOnly

                    onClicked: root.sourcesListVisible = !root.sourcesListVisible

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                    Layout.minimumWidth: Layout.preferredWidth
                }
            }
        }

        ListView {
            id: sources

            model: PlasmaTube.sourceManager
            currentIndex: PlasmaTube.sourceManager.selectedIndex

            header: Kirigami.Separator {}

                    right: parent.right
                    rightMargin: Kirigami.Units.smallSpacing
                }
            }

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
                    root.sourcesListVisible = false
                }

                Keys.onUpPressed: {
                    sources.currentIndex = accounts.count - 1
                    sources.forceActiveFocus()
                }
                Keys.onDownPressed: {
                    sources.currentIndex = 0
                    sources.forceActiveFocus()
                }
            }

            visible: false
            onVisibleChanged: if (visible) focus = true
            clip: true

            keyNavigationEnabled: false
            Keys.onDownPressed: {
                if (sources.currentIndex === sources.count - 1) {
                    addAccount.forceActiveFocus()
                    sources.currentIndex = -1
                } else {
                    sources.incrementCurrentIndex()
                }
            }
            Keys.onUpPressed: {
                if (sources.currentIndex === 0) {
                    addAccount.forceActiveFocus()
                    sources.currentIndex = -1
                } else {
                    sources.decrementCurrentIndex()
                }
            }

            Keys.onReleased: if (event.key == Qt.Key_Escape) {
                root.sourcesListVisible = false
            }

            Layout.fillWidth: true
            Layout.preferredHeight: contentHeight

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
                    }
                    root.sourcesListVisible = false
                }
            }
        }

    }
}
