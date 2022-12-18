// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
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

    GridView {
        id: gridView
        topMargin: root.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
        bottomMargin: root.width > 900 ? Kirigami.Units.gridUnit * 2 : Kirigami.Units.largeSpacing
        leftMargin: root.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing
        rightMargin: root.width > 900 ? Kirigami.Units.gridUnit * 4 : Kirigami.Units.largeSpacing

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

        readonly property real effectiveWidth: width - leftMargin - rightMargin
        readonly property real targetDelegateWidth: Kirigami.Units.gridUnit * 14 + Kirigami.Units.largeSpacing * 2
        readonly property int columns: Math.floor(effectiveWidth / targetDelegateWidth)

        cellWidth: effectiveWidth / columns
        cellHeight: (cellWidth / 16 * 9) + Kirigami.Units.gridUnit * 4

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

            onClicked: {
                applicationWindow().switchVideo(vid);
                applicationWindow().openPlayer();
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

