// SPDX-FileCopyrightText: none
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiComponents

import org.kde.plasmatube
import "components/utils.js" as Utils
import "components"

Kirigami.ScrollablePage {
    id: root

    required property string author
    required property string authorId

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

    header: KirigamiComponents.Banner {
        id: message
        type: Kirigami.MessageType.Error
        width: parent.width

        showCloseButton: true

        actions: Kirigami.Action {
            text: i18n("Proxy Settings")
            icon.name: "settings-configure"
            onTriggered: pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "NetworkProxyPage"))
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    ChannelController {
        id: channelController
    }

    BaseGridView {
        id: gridView

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
            QQC2.TextArea {
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

        model: VideoListModel {
            id: videoModel
        }
    }

    Component.onCompleted: {
        videoModel.requestChannel(authorId);
        channelController.loadChannel(authorId);
    }
}
