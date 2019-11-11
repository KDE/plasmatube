/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as Controls
import QtGraphicalEffects 1.12
import QtMultimedia 5.12

import org.kde.kirigami 2.10 as Kirigami

import org.kde.plasmatube.models 1.0
import org.kde.plasmatube.ytmusic 1.0 as YTM

Kirigami.ScrollablePage {
    id: root
    title: qsTr("YouTube Music")
    supportsRefreshing: true
    onRefreshingChanged: {
        if (refreshing)
            searchModel.tryStartFetching();
    }

    leftAction: Kirigami.Action {
        visible: searchModel.canGoBack
        enabled: !searchModel.isLoading
        text: qsTr("Go back")
        icon.name: "arrow-left"
        onTriggered: {
            searchModel.goBack();
            root.refreshing = true;
        }
    }

    rightAction: Kirigami.Action {
        visible: searchModel.canGoForward
        enabled: !searchModel.isLoading
        text: qsTr("Go forward")
        icon.name: "arrow-right"
        onTriggered: {
            searchModel.goForward();
            root.refreshing = true;
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    header: Rectangle {
        color: Kirigami.Theme.backgroundColor
        height: searchField.implicitHeight + 2 * Kirigami.Units.largeSpacing
        width: root.width

        Kirigami.SearchField {
            id: searchField
            anchors.centerIn: parent
            anchors.margins: Kirigami.Units.largeSpacing
            width: parent.width - 2 * Kirigami.Units.largeSpacing

            selectByMouse: true
            rightActions: [
                Kirigami.Action {
                    icon.name: "search"
                    onTriggered: searchField.accepted()
                }
            ]

            onAccepted: {
                searchModel.search(searchField.text, "");
                root.refreshing = true;
            }
        }
    }

    footer: Controls.Pane {
        height: 75
        
        layer.enabled: true
        layer.effect: DropShadow {
            verticalOffset: 1
            color: Kirigami.Theme.disabledTextColor
            samples: 20
            spread: 0.3
            cached: true
        }
        padding: 0
        wheelEnabled: true
        background: Rectangle {
            color: Kirigami.Theme.backgroundColor
        }

        VideoModel {
            id: currentTrack
            onVideoIdChanged: {
                // is also executed in initial set
                currentTrack.fetch()
            }
        }

        Audio {
            id: player
            source: currentTrack.video.audioUrl()
            audioRole: Audio.MusicRole
            autoPlay: true
        }

        Rectangle {
            width: root.width
            height: 4
            color: Qt.darker(Kirigami.Theme.disabledTextColor, 1.2)
        }

        Rectangle {
            width: root.width * player.bufferProgress
            height: 4
            color: Qt.lighter(Kirigami.Theme.disabledTextColor, 1.1)
        }
        
        Rectangle {
            width: root.width * (player.position / player.duration)
            height: 4
            color: Kirigami.Theme.highlightColor
        }

        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing * 2

            Kirigami.Icon {
                height: parent.height
                width: height
                source: player.metaData.coverArtUrlLarge ? player.metaData.coverArtUrlLarge : "media-default-album"
            }

            ColumnLayout {
                Layout.fillWidth: true

                Controls.Label {
//                     text: "<b>Enjoy The Silence</b>"
                    text: "<b>" + currentTrack.video.title + "</b>"
                }

                Controls.Label {
//                     text: "Depeche Mode"
                    text: currentTrack.video.author
//                     text: player.metaData.author
                    color: Kirigami.Theme.disabledTextColor
                }

                Item {
                    Layout.fillWidth: true
                }
            }

            Kirigami.Icon {
                height: parent.height * 0.3
                width: height
                source: "media-skip-backward"
            }

            Controls.ToolButton {
                height: parent.height * 0.8
                width: height
                onClicked: {
                    if (player.playbackState === Audio.PlayingState)
                        player.pause();
                    else
                        player.play();
                }

                Kirigami.Icon {
                    anchors.centerIn: parent
                    height: parent.parent.height * 0.5
                    width: height
                    source: {
                        if (player.playbackState === Audio.PlayingState)
                            return "media-playback-pause";
                        else
                            return "media-playback-start";
                    }
                }
            }

            Kirigami.Icon {
                height: parent.height * 0.3
                width: height
                source: "media-skip-forward"
            }

            Item {
                height: parent.height
            }
        }
    }

    ListView {
        clip: true
        model: YTM.SearchModel {
            id: searchModel
            onIsLoadingChanged: {
                if (!isLoading)
                    root.refreshing = false;
            }
        }

        delegate: Component {
            Loader {
                property int currentIndex: index
                property var modelData: model

                width: root.width
                height: item ? item.height : undefined

                sourceComponent: {
                    switch (model.type) {
                    case YTM.SearchModel.Item:
                        return musicItemComponent;
                    case YTM.SearchModel.Shelf:
                        return shelfComponent;
                    case YTM.SearchModel.SearchEndpoint:
                        return showAllComponent;
                    }
                }
            }
        }

        Component {
            id: shelfComponent

            Kirigami.ListSectionHeader {
                visible: modelData.shelfTitle.length > 0
                text: modelData.shelfTitle
            }
        }

        Component {
            id: musicItemComponent

            Kirigami.BasicListItem {
                reserveSpaceForIcon: false
                reserveSpaceForLabel: false

                onClicked: {
                    showPassiveNotification("VIDEO ID: " + modelData.videoId)
                    if (modelData.videoId)
                        currentTrack.videoId = modelData.videoId;
                }

                RowLayout {
                    spacing: Kirigami.Units.largeSpacing

                    Image {
                        Layout.preferredWidth: 42
                        Layout.preferredHeight: 42

                        source: modelData.thumbnailUrl
                        fillMode: Image.PreserveAspectCrop
                        mipmap: true
                    }

                    ColumnLayout {
                        Controls.Label {
                            text: modelData.title
                        }

                        Controls.Label {
                            text: modelData.attributes.join(" \u2022 ")
                        }
                    }
                }
            }
        }

        Component {
            id: showAllComponent

            Controls.Button {
                text: qsTr("Show all")
                onClicked: {
                    searchModel.searchByEndpointIndex(currentIndex);
                    root.refreshing = true;
                }
                flat: true
            }
        }
    }
}
