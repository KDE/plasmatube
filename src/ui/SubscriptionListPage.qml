// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Window
import Qt5Compat.GraphicalEffects

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as Components

import org.kde.plasmatube
import org.kde.plasmatube.private

Kirigami.ScrollablePage {
    id: root

    required property var source

    title: i18nc("@title:window List of subscribed channels", "Subscribed Channels")

    ListView {
        id: listview

        model: SubscriptionListModel {}
        currentIndex: -1

        delegate: QQC2.ItemDelegate {
            id: delegate

            required property var index
            required property string name
            required property string channelId
            required property string avatar
            required property int subCount

            width: ListView.view.width

            onClicked: pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "ChannelPage"), {author: name, authorId: channelId})

            contentItem: RowLayout {
                spacing: Kirigami.Units.largeSpacing

                Image {
                    id: chanelThumb
                    Layout.preferredHeight: 50
                    Layout.preferredWidth: 50
                    fillMode: Image.PreserveAspectFit
                    source: avatar
                    layer.enabled: true
                    layer.effect: OpacityMask {
                        maskSource: mask
                    }
                    Rectangle {
                        id: mask
                        radius: chanelThumb.height/2
                        anchors.fill: chanelThumb
                        visible: false
                    }
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.goToChannel()
                    }
                }

                QQC2.Label {
                    text: name
                    font.weight: Font.Bold
                }

                Item {
                    Layout.fillWidth: true
                }

                SubscriptionButton {
                    id: subscribeButton

                    channelId: delegate.channelId
                    subCountText: subCount
                }
            }
        }

        QQC2.ProgressBar {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
            indeterminate: true
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No channels")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }

    Component.onCompleted: listview.model.fill(root.source)
}
