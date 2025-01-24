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

Kirigami.Page {
    id: root

    required property string author
    required property string authorId

    title: author

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    header: Kirigami.InlineMessage {
        id: message
        type: Kirigami.MessageType.Error
        width: parent.width
        position: Kirigami.InlineMessage.Position.Header

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

    ColumnLayout {
        anchors.fill: parent

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
                subCount: channelController.subCount
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

        QQC2.TabBar {
            id: bar

            // Hack to disable the qqc2-desktop-style scrolling behavior.
            // This bar is on a scrollable page, you will eventually run into this tab bar which is annoying.
            background: null

            Layout.fillWidth: true

            QQC2.TabButton {
                text: i18nc("@item:inmenu", "Videos")

                implicitWidth: bar.parent.width / 2
            }

            QQC2.TabButton {
                text: i18nc("@item:inmenu", "Playlists")

                implicitWidth: bar.parent.width / 2
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: bar.currentIndex

            onCurrentIndexChanged: {
                // TODO: This depends on the channel actually having any playlists.
                // I guess it's harmless to call this twice, but it really shouldn't happen.
                if (currentIndex === 1 && playlistModel.rowCount() === 0) {
                    playlistModel.loadChannelPlaylists(root.authorId);
                }
            }

            QQC2.ScrollView {
                clip: true
                
                QQC2.ScrollBar.horizontal: QQC2.ScrollBar {
                    policy: QQC2.ScrollBar.AlwaysOff
                }

                BaseGridView {
                    id: videoGridView

                    placeholderIconName: "videoclip-amarok"
                    placeholderText: i18nc("@info:placeholder", "No Videos")
                    model: VideoListModel {
                        id: videoModel
                    }
                }
            }

            QQC2.ScrollView {
                clip: true

                QQC2.ScrollBar.horizontal: QQC2.ScrollBar {
                    policy: QQC2.ScrollBar.AlwaysOff
                }

                BaseGridView {
                    id: playlistsGridView

                    placeholderIconName: "view-media-playlist"
                    placeholderText: i18nc("@info:placeholder", "No Playlists")
                    model: PlaylistsModel {
                        id: playlistModel
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        videoModel.requestChannel(authorId);
        channelController.loadChannel(authorId);
    }
}
