// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

import org.kde.plasmatube
import "components"

Kirigami.ScrollablePage {
    id: root
    title: videoModel.title
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    property var initialQuery: VideoListModel.Trending
    property alias videoModel: videoModel

    supportsRefreshing: true
    onRefreshingChanged: {
        if (refreshing && !videoModel.isLoading) {
            videoModel.refresh();
        }
    }

    header: Components.Banner {
        id: message

        type: Kirigami.MessageType.Error
        showCloseButton: true

        actions: Kirigami.Action {
            text: i18n("Proxy Settings")
            icon.name: "settings-configure"
            onTriggered: pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "NetworkProxyPage"))
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    BaseGridView {
        id: gridView

        model: VideoListModel {
            id: videoModel
        }
    }
}

