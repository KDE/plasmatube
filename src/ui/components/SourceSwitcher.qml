// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.plasmatube

QQC2.Pane {
    id: root

    readonly property var selectedSource: PlasmaTube.sourceManager.selectedSource

    padding: 0

    contentItem: ColumnLayout {
        id: content

        spacing: 0

        QQC2.Control {
            id: currentSourceDelegate

            Layout.fillWidth: true

            contentItem: RowLayout {
                spacing: 0

                ColumnLayout {
                    spacing: 0

                    QQC2.Label {
                        id: labelItem

                        text: root.selectedSource?.url
                        horizontalAlignment: Text.AlignLeft
                        textFormat: Text.PlainText
                    }

                    QQC2.Label {
                        id: subtitleItem

                        text: {
                            switch (root.selectedSource?.type) {
                                case VideoSource.Invidious:
                                    return i18n("Invidious");
                                case VideoSource.PeerTube:
                                    return i18n("PeerTube");
                                case VideoSource.Piped:
                                    return i18n("Piped");
                            }
                        }
                        font: Kirigami.Theme.smallFont
                        color: Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.textColor, Kirigami.Theme.backgroundColor, 0.3)
                        visible: text.length > 0
                        horizontalAlignment: Text.AlignLeft
                        textFormat: Text.PlainText
                    }
                }

                QQC2.ToolButton {
                    text: i18n("Switch Source")
                    icon.name: "system-switch-user"

                    display: QQC2.AbstractButton.IconOnly

                    onClicked: sourceSwitcherDialog.createObject(QQC2.ApplicationWindow.overlay).open()

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                    Layout.minimumWidth: Layout.preferredWidth
                    Layout.alignment: Qt.AlignRight
                }
            }
        }
    }

    Component {
        id: sourceSwitcherDialog
        SourceSwitcherDialog {}
    }
}
