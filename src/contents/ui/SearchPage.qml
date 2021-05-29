// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.8 as Kirigami

import org.kde.plasmatube 1.0
import org.kde.plasmatube.models 1.0
import "utils.js" as Utils

Kirigami.ScrollablePage {
    id: root
    title: videoModel.title
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

    actions.contextualActions: [
        VideoListAction {
            visible: PlasmaTube.isLoggedIn
            videoModel: videoModel
            queryType: VideoListModel.Feed
        },
        VideoListAction {
            videoModel: videoModel
            queryType: VideoListModel.Top
        },
        VideoListAction {
            videoModel: videoModel
            queryType: VideoListModel.Trending
        },
        VideoListAction {
            videoModel: videoModel
            queryType: VideoListModel.TrendingGaming
        },
        VideoListAction {
            videoModel: videoModel
            queryType: VideoListModel.TrendingMovies
        },
        VideoListAction {
            videoModel: videoModel
            queryType: VideoListModel.TrendingMusic
        },
        VideoListAction {
            videoModel: videoModel
            queryType: VideoListModel.TrendingNews
        }
    ]

    header: Rectangle {
        color: Kirigami.Theme.backgroundColor
        height: searchField.implicitHeight + 2 * Kirigami.Units.largeSpacing
        width: root.width

        Kirigami.SearchField {
            id: searchField
            selectByMouse: true
            anchors.centerIn: parent
            anchors.margins: Kirigami.Units.largeSpacing
            width: parent.width - 2 * Kirigami.Units.largeSpacing
            delaySearch: true

            onAccepted: {
                videoModel.requestSearchResults(text)
            }

            rightActions: [
                Kirigami.Action {
                    icon.name: "search"
                    onTriggered: searchField.accepted()
                }
            ]
        }
    }

    ListView {
        clip: true
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

        Connections {
            target: PlasmaTube

            function onLoggedIn() {
                videoModel.requestQuery(VideoListModel.Feed)
            }
            function onLoggedOut() {
                videoModel.requestQuery(VideoListModel.Trending)
            }
        }
    }

    Component.onCompleted: {
        if (PlasmaTube.isLoggedIn)
            videoModel.requestQuery(VideoListModel.Feed)
        else
            videoModel.requestQuery(VideoListModel.Trending)
    }
}
