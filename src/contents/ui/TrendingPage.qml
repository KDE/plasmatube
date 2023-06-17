// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
//
// SPDX-License-Identifier: GPL-3.0-or-later

import org.kde.plasmatube.models 1.0

FeedPage {
    id: page

    initialQuery: VideoListModel.Trending

    actions.contextualActions: [
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
}