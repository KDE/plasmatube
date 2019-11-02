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

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.8 as Kirigami

import org.kde.plasmatube.accountmanager 1.0
import org.kde.plasmatube.models 1.0
import org.kde.plasmatube.invidious 1.0
import org.kde.plasmatube.youtubemusic 1.0
import "utils.js" as Utils

Kirigami.ScrollablePage {
    id: root
    title: videoListTypeToString(videoModel.queryType, videoModel.query)
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    supportsRefreshing: true
    onRefreshingChanged: {
        if (refreshing) {
//             videoModel.fetch();
            ytmusic.musicInfo("Lindemann");
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    YouTubeMusicManager {
        id: ytmusic
    }

    actions.contextualActions: [
        Kirigami.Action {
            visible: AccountManager.username.length > 0
            text: videoListTypeToString(InvidiousManager.Feed)
            icon.name: "feed-subscribe"
            onTriggered: {
                videoModel.queryType = InvidiousManager.Feed;
                videoModel.query = "";
                root.refreshing = true;
            }
        },
        Kirigami.Action {
            text: videoListTypeToString(InvidiousManager.Top)
            iconName: "arrow-up-double"
            onTriggered: {
                videoModel.queryType = InvidiousManager.Top
                videoModel.query = ""
                root.refreshing = true;
            }
        },
        Kirigami.Action {
            text: Utils.trendingToString()
            iconName: "favorite" // should actually be "user-trash-full-symbolic"
            onTriggered: {
                videoModel.queryType = InvidiousManager.Trending
                videoModel.query = ""
                root.refreshing = true;
            }
        },
        Kirigami.Action {
            text: Utils.trendingToString("music")
            iconName: "folder-music-symbolic"
            onTriggered: {
                videoModel.queryType = InvidiousManager.Trending
                videoModel.query = "music"
                root.refreshing = true;
            }
        },
        Kirigami.Action {
            text: Utils.trendingToString("gaming")
            iconName: "folder-games-symbolic"
            onTriggered: {
                videoModel.queryType = InvidiousManager.Trending
                videoModel.query = "gaming"
                root.refreshing = true;
            }
        },
        Kirigami.Action {
            text: Utils.trendingToString("news")
            iconName: "message-news"
            onTriggered: {
                videoModel.queryType = InvidiousManager.Trending
                videoModel.query = "news"
                root.refreshing = true;
            }
        },
        Kirigami.Action {
            text: Utils.trendingToString("movies")
            iconName: "folder-videos-symbolic"
            onTriggered: {
                videoModel.queryType = InvidiousManager.Trending
                videoModel.query = "movies"
                root.refreshing = true;
            }
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

            onAccepted: {
                videoModel.queryType = InvidiousManager.Search;
                videoModel.query = text;
                root.refreshing = true;
            }

            rightActions: [
                Kirigami.Action {
                    iconName: "search"
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
                if (!isLoading)
                    root.refreshing = false;
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
        }
    }

    Component.onCompleted: {
        if (AccountManager.username.length > 0)
            videoModel.queryType = InvidiousManager.Feed;
        else
            videoModel.queryType = InvidiousManager.Trending;
        root.refreshing = true;
    }

    function videoListTypeToString(videoListType, query) {
        switch (videoListType) {
        case InvidiousManager.Feed:
            return qsTr("Subscriptions");
        case InvidiousManager.Trending:
            return Utils.trendingToString(query);
        case InvidiousManager.Top:
            return qsTr("Invidious Top");
        case InvidiousManager.Search:
            return qsTr("Search");
        }
    }
}
