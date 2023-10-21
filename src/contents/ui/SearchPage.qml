// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

import org.kde.plasmatube
import org.kde.plasmatube.invidious

Kirigami.ScrollablePage {
    id: root
    title: i18n("Search")
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    property string initialSearch

    supportsRefreshing: true
    onRefreshingChanged: {
        if (refreshing && !videoModel.isLoading) {
            videoModel.refresh();
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    Component.onCompleted: {
        if (initialSearch.length !== 0) {
            searchField.text = initialSearch;
            searchField.accepted();
        }
    }

    header: QQC2.Control {
        padding: Kirigami.Units.largeSpacing

        background: Rectangle {
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false

            color: Kirigami.Theme.backgroundColor
        }

        anchors.top: root.top
        anchors.left: root.left

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.mediumSpacing

            RowLayout {
                spacing: Kirigami.Units.mediumSpacing

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

                QQC2.Button {
                    id: showFiltersButton
                    icon.name: "settings-configure"
                    checkable: true
                    checked: false
                }
            }

            RowLayout {
                spacing: Kirigami.Units.mediumSpacing

                id: filtersBar
                visible: showFiltersButton.checked

                QQC2.ButtonGroup { id: sortGroup }

                QQC2.Label {
                    text: i18n("Sort By:")
                }

                QQC2.RadioButton {
                    text: i18n("Rating")
                    checkable: true
                    QQC2.ButtonGroup.group: sortGroup
                    onCheckedChanged:
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.Rating
                            videoModel.requestSearchResults(searchParameters)
                        }
                }

                QQC2.RadioButton {
                    text: i18n("Relevance")
                    checkable: true
                    checked: true
                    QQC2.ButtonGroup.group: sortGroup
                    onCheckedChanged:
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.Relevance
                            videoModel.requestSearchResults(searchParameters)
                        }
                }

                QQC2.RadioButton {
                    text: i18n("Upload Date")
                    checkable: true
                    QQC2.ButtonGroup.group: sortGroup
                    onCheckedChanged:
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.UploadDate
                            videoModel.requestSearchResults(searchParameters)
                        }
                }

                QQC2.RadioButton {
                    text: i18n("View Count")
                    checkable: true
                    QQC2.ButtonGroup.group: sortGroup
                    onCheckedChanged:
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.ViewCount
                            videoModel.requestSearchResults(searchParameters)
                        }
                }
            }
        }

        Kirigami.Separator {
            z: 999
            anchors {
                left: parent.left
                right: parent.right
                top: parent.bottom
            }
        }
    }

    SearchParameters {
        id: searchParameters
        query: ""
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

            onClicked: PlasmaTube.videoController.play(vid)
        }
        
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: gridView.count === 0 && !root.refreshing
            text: searchField.text === "" ? i18n("Search") : i18n("No results")
            icon.name: "search"
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18nc("@info:status", "Loading…")
            visible: gridView.model.isLoading
        }
    }
}
