// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

import org.kde.kirigami as Kirigami

import org.kde.plasmatube

FeedPage {
    id: page

    initialQuery: VideoListModel.Trending

    title: i18nc("@title Trending videos", "Trending")

    header: Kirigami.NavigationTabBar {
        width: parent.width
        actions: [
            VideoListAction {
                videoModel: page.videoModel
                queryType: VideoListModel.Trending
            },
            VideoListAction {
                videoModel: page.videoModel
                queryType: VideoListModel.TrendingGaming
            },
            VideoListAction {
                videoModel: page.videoModel
                queryType: VideoListModel.TrendingMovies
            },
            VideoListAction {
                videoModel: page.videoModel
                queryType: VideoListModel.TrendingMusic
            },
            VideoListAction {
                videoModel: page.videoModel
                queryType: VideoListModel.TrendingNews
            }
        ]

        Component.onCompleted: actions[0].checked = true
    }
}