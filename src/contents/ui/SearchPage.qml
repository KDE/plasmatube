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
    title: "Search"
    header: RowLayout {
        Controls.TextField {
            id: searchField
            Layout.fillWidth: true
        }
        Controls.ToolButton {
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

        Controls.BusyIndicator {
            visible: searchModel.isLoading
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: SearchModel {
                id: searchModel
            }
            delegate: Kirigami.BasicListItem {
                onClicked: {
                    pageStack.push(videoPageComponent, {"url": "ytdl://" + model.id})
                }

                RowLayout {
                    Image {
                        Layout.preferredWidth: 224
                        Layout.preferredHeight: 120
                        source: model.thumbnail
                        fillMode: Image.PreserveAspectFit
                    }

                    ColumnLayout {
                        Kirigami.Heading {
                            Layout.fillWidth: true
                            text: model.title
                            level: 3
                        }
                        Kirigami.Heading {
                            Layout.fillWidth: true
                            text: model.description
                            level: 6
                            wrapMode: Text.Wrap
                        }
                    }
                }
            }
        }
    }
}
