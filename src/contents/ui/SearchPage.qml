// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasmatube 1.0
import org.kde.plasmatube.models 1.0
import "utils.js" as Utils

Kirigami.ScrollablePage {
    id: root
    title: i18n("Search")
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

    header: Rectangle {
        color: "transparent"
        anchors.top: root.top
        anchors.left: root.left
        anchors.margins: Kirigami.Units.largeSpacing
        height: childrenRect.height + Kirigami.Units.largeSpacing

        ColumnLayout {
            width: root.width - 2 * Kirigami.Units.largeSpacing

            RowLayout {
                Kirigami.SearchField {
                    id: searchField
                    selectByMouse: true
                    delaySearch: true
                    Layout.fillWidth: true

                    onAccepted: {
                        searchParameters.query = text
                        videoModel.requestSearchResults(searchParameters)
                    }
                }

                Controls.Button {
                    id: showFiltersButton
                    icon.name: "settings-configure"
                    checkable: true
                    checked: false
                }
            }

            RowLayout {
                id: filtersBar
                visible: showFiltersButton.checked

                Controls.ButtonGroup { id: sortGroup }

                Controls.Label {
                    text: i18n("Sort By:")
                }

                Controls.RadioButton {
                    text: i18n("Rating")
                    checkable: true
                    Controls.ButtonGroup.group: sortGroup
                    onCheckedChanged:
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.Rating
                            videoModel.requestSearchResults(searchParameters)
                        }
                }

                Controls.RadioButton {
                    text: i18n("Relevance")
                    checkable: true
                    checked: true
                    Controls.ButtonGroup.group: sortGroup
                    onCheckedChanged:
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.Relevance
                            videoModel.requestSearchResults(searchParameters)
                        }
                }

                Controls.RadioButton {
                    text: i18n("Upload Date")
                    checkable: true
                    Controls.ButtonGroup.group: sortGroup
                    onCheckedChanged:
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.UploadDate
                            videoModel.requestSearchResults(searchParameters)
                        }
                }

                Controls.RadioButton {
                    text: i18n("View Count")
                    checkable: true
                    Controls.ButtonGroup.group: sortGroup
                    onCheckedChanged:
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.ViewCount
                            videoModel.requestSearchResults(searchParameters)
                        }
                }
            }
        }
    }

    SearchParameters {
        id: searchParameters
        query: ""
    }

    ListView {
        id: listView
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
        delegate: VideoListItem {
            vid: model.id
            thumbnail: model.thumbnail
            liveNow: model.liveNow
            length: model.length
            title: model.title
            author: model.author
            description: model.description
            viewCount: model.viewCount
            publishedText: model.publishedText

            onClicked: {
                pageStack.push(videoPageComponent, {"vid": vid})
            }
        }
        
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: listView.count === 0 && !root.refreshing
            text: searchField.text === "" ? i18n("Search") : i18n("No results")
            icon.name: "search"
        }
    }
}
