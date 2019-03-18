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
import org.kde.kirigami 2.4 as Kirigami
import org.kde.plasmatube 1.0

Kirigami.Page {
    id: root
    title: "PlasmaTube"
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    header: RowLayout {
        Rectangle {
            anchors.fill: parent
            color: Kirigami.Theme.backgroundColor
        }

        Controls.TextField {
            id: searchField
            selectByMouse: true
            placeholderText: "Search"
            Layout.fillWidth: true
        }
        Controls.Button {
            Kirigami.Icon {
                anchors.fill: parent
                source: "search"
            }

            onPressed: {
                searchModel.searchQuery = searchField.text
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: SearchModel {
                id: searchModel
            }
            delegate: Kirigami.AbstractListItem {
                onClicked: {
                    pageStack.push(videoPageComponent, {"url": "ytdl://" + model.id})
                }

                RowLayout {
                    Image {
                        id: thumb
                        Layout.fillHeight: true
                        Layout.preferredWidth: 208
                        Layout.preferredHeight: 111
                        Layout.maximumWidth: root.width * 0.3
                        Layout.maximumHeight: root.width * 0.3 / 16.0 * 9.0
                        source: model.thumbnail
                        fillMode: Image.PreserveAspectFit
                    }

                    ColumnLayout {
                        id: videoInfo
                        Layout.preferredHeight: thumb.height
                        Layout.maximumHeight: root.width * 0.3 / 16.0 * 9.0
                        Layout.minimumHeight: 70
                        spacing: 0
                        Kirigami.Heading {
                            Layout.alignment: Qt.AlignTop
                            Layout.fillWidth: true

                            text: model.title
                            level: 4
                            maximumLineCount: 1
                            elide: Text.ElideRight
                        }
                        Controls.Label {
                            Layout.alignment: Qt.AlignTop
                            Layout.fillWidth: true

                            text: model.author
                            font.pointSize: 9
                            font.italic: true
                            maximumLineCount: 1
                            elide: Text.ElideRight
                        }
                        Controls.Label {
                            visible: root.width > 580
                            enabled: root.width > 580
                            Layout.alignment: Qt.AlignTop
                            Layout.fillWidth: true

                            text: model.description
                            font.pointSize: 8
                            maximumLineCount: 2
                            wrapMode: Text.Wrap
                            elide: Text.ElideRight
                        }
                        Controls.Label {
                            Layout.alignment: Qt.AlignBottom
                            text: formatViewCount(model.viewCount) +
                                  " views \u2022 " + model.publishedText
                            font.pointSize: 9
                        }
                    }
                }
            }
        }

        Controls.BusyIndicator {
            Layout.alignment: Qt.AlignCenter
            visible: searchModel.isLoading
        }
    }

    function formatViewCount(count, decimals) {
       if (count === 0)
           return '0';
       var k = 1000,
           dm = decimals <= 0 ? 0 : decimals || 0,
           sizes = ['', 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'],
           i = Math.floor(Math.log(count) / Math.log(k));
       return parseFloat((count / Math.pow(k, i)).toFixed(dm)) + sizes[i];
    }
}
