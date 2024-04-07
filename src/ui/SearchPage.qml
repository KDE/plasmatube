// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

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
                        searchParameters.query = text;
                        videoModel.request(searchParameters);
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
                    onCheckedChanged: {
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.Rating;
                            videoModel.request(searchParameters);
                        }
                    }
                }

                QQC2.RadioButton {
                    text: i18n("Relevance")
                    checkable: true
                    checked: true
                    QQC2.ButtonGroup.group: sortGroup
                    onCheckedChanged: {
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.Relevance;
                            videoModel.request(searchParameters);
                        }
                    }
                }

                QQC2.RadioButton {
                    text: i18n("Upload Date")
                    checkable: true
                    QQC2.ButtonGroup.group: sortGroup
                    onCheckedChanged: {
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.UploadDate;
                            videoModel.request(searchParameters);
                        }
                    }
                }

                QQC2.RadioButton {
                    text: i18n("View Count")
                    checkable: true
                    QQC2.ButtonGroup.group: sortGroup
                    onCheckedChanged: {
                        if (checked) {
                            searchParameters.sortBy = SearchParameters.SortBy.ViewCount;
                            videoModel.request(searchParameters);
                        }
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

        Components.Banner {
            id: message
            type: Kirigami.MessageType.Error
            width: parent.width

            showCloseButton: true

            actions: Kirigami.Action {
                text: i18n("Proxy Settings")
                icon.name: "settings-configure"
                onTriggered: pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "NetworkProxyPage"))
            }
        }
    }

    SearchParameters {
        id: searchParameters
        query: ""
    }

    BaseGridView {
        id: gridView

        model: SearchModel {
            id: videoModel
        }
    }
}
