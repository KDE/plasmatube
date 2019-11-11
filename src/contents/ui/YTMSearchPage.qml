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
import org.kde.kirigami 2.10 as Kirigami

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
