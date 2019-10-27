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
import org.kde.kirigami 2.7 as Kirigami
import org.kde.plasmatube 1.0
import "utils.js" as Utils

Kirigami.Page {
    id: root
    title: videoModel.trending ? Utils.trendingToString(videoModel.trendingCategory)
                               : "Search"
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    actions.contextualActions: [
        Kirigami.Action {
            text: "Trending"
            iconName: "favorite" // should actually be "user-trash-full-symbolic"
            onTriggered: {
                videoModel.trending = true
                videoModel.trendingCategory = ""
                videoModel.fetch()
            }
        },
        Kirigami.Action {
            text: "Trending Music"
            iconName: "folder-music-symbolic"
            onTriggered: {
                videoModel.trending = true
                videoModel.trendingCategory = "music"
                videoModel.fetch()
            }
        },
        Kirigami.Action {
            text: "Trending Gaming"
            iconName: "folder-games-symbolic"
            onTriggered: {
                videoModel.trending = true
                videoModel.trendingCategory = "gaming"
                videoModel.fetch()
            }
        },
        Kirigami.Action {
            text: "Trending News"
            iconName: "message-news"
            onTriggered: {
                videoModel.trending = true
                videoModel.trendingCategory = "news"
                videoModel.fetch()
            }
        },
        Kirigami.Action {
            text: "Trending Movies"
            iconName: "folder-videos-symbolic"
            onTriggered: {
                videoModel.trending = true
                videoModel.trendingCategory = "movies"
                videoModel.fetch()
            }
        }
    ]

    header: RowLayout {
        Rectangle {
            anchors.fill: parent
            color: Kirigami.Theme.backgroundColor
        }

        Kirigami.ActionTextField {
            id: searchField
            selectByMouse: true
            placeholderText: "Search"
            Layout.fillWidth: true
            rightActions:  [
                Kirigami.Action {
                    iconName: "search"
                    onTriggered: {
                        videoModel.searchQuery = searchField.text
                        videoModel.fetch()
                    }
                }
            ]
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: VideoListModel {
                id: videoModel
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

        Controls.BusyIndicator {
            Layout.alignment: Qt.AlignCenter
            visible: videoModel.isLoading
        }
    }

    Component.onCompleted: {
        videoModel.trending = true
        videoModel.fetch()
    }
}
