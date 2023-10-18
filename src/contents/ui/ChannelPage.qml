// SPDX-FileCopyrightText: none
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import org.kde.plasmatube 1.0
import "utils.js" as Utils
import "components"

Kirigami.ScrollablePage {
    property string author
    property string authorId

    id: root
    title: author
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    supportsRefreshing: true
    onRefreshingChanged: {
        if (refreshing && !videoModel.isLoading) {
            videoModel.refresh();
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    ChannelController {
        id: channelController
    }

    GridView {
        id: gridView
        topMargin: root.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
        bottomMargin: root.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
        leftMargin: root.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing
        rightMargin: root.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing

        readonly property real effectiveWidth: width - leftMargin - rightMargin
        readonly property real targetDelegateWidth: Kirigami.Units.gridUnit * 14 + Kirigami.Units.largeSpacing * 2
        readonly property int columns: Math.floor(effectiveWidth / targetDelegateWidth)

        cellWidth: effectiveWidth / columns
        cellHeight: (cellWidth / 16 * 9) + Kirigami.Units.gridUnit * 4

        header: ColumnLayout {
            width: parent.width

            spacing: 0

            Rectangle {
                Layout.preferredHeight: Kirigami.Units.gridUnit * 7
                Layout.fillWidth: true
                clip: true
                color: Kirigami.Theme.backgroundColor
                Kirigami.Theme.colorSet: Kirigami.Theme.View

                Image {
                    anchors.centerIn: parent
                    source: channelController.banner
                    fillMode: Image.PreserveAspectFit
                }
            }

            RowLayout {
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                Item {
                    Layout.minimumWidth: Kirigami.Units.gridUnit * 5
                    Layout.alignment: Qt.AlignBottom
                    KirigamiComponents.Avatar {
                        anchors {
                            left: parent.left
                            bottom: parent.bottom
                        }
                        cache: true
                        source: channelController.avatar
                        width: Kirigami.Units.gridUnit * 5
                        height: Kirigami.Units.gridUnit * 5
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                SubscriptionButton {
                    channelId: root.authorId
                    subCountText: Utils.formatCount(channelController.subCount)
                }
            }

            Kirigami.Heading {
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                text: channelController.name
            }
            Controls.TextArea {
                text: channelController.description
                textFormat: TextEdit.RichText
                readOnly: true
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                leftPadding: 0
                rightPadding: 0
                bottomPadding: 0
                topPadding: 0
                background: null
                wrapMode: Text.WordWrap
                onLinkActivated: (link) => applicationWindow().navigateLink(link, true)
                onHoveredLinkChanged: if (hoveredLink.length > 0) {
                    applicationWindow().hoverLinkIndicator.text = hoveredLink;
                } else {
                    applicationWindow().hoverLinkIndicator.text = "";
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }

        currentIndex: -1
        model: VideoListModel {
            id: videoModel
            onIsLoadingChanged: {
                root.refreshing = isLoading
            }
            onErrorOccured: (errorText) => {
                applicationWindow().showPassiveNotification(errorText)
            }
        }
        delegate: VideoGridItem {
            width: gridView.cellWidth
            height: gridView.cellHeight

            vid: model.id
            thumbnail: model.thumbnail
            liveNow: model.liveNow
            length: model.length
            title: model.title
            author: model.author
            authorId: model.authorId
            description: model.description
            viewCount: model.viewCount
            publishedText: model.publishedText
            watched: model.watched

            onClicked: {
                applicationWindow().switchVideo(vid);
                applicationWindow().openPlayer();
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: gridView.count === 0 && !root.refreshing
            text: i18nc("@info:status", "Loading…")
        }
    }

    Component.onCompleted: {
        videoModel.requestChannel(authorId);
        channelController.loadChannel(authorId);
    }
}
