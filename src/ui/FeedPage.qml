// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

import org.kde.kirigami as Kirigami

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

    Connections {
        target: gridView.model

        function onErrorOccured(errorText) {
            message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your proxy settings.", errorText);
            message.visible = true;
        }
    }

    header: Kirigami.InlineMessage {
        id: message

        type: Kirigami.MessageType.Error
        showCloseButton: true
        position: Kirigami.InlineMessage.Position.Header

        actions: Kirigami.Action {
            text: i18n("Proxy Settings")
            icon.name: "settings-configure"
            onTriggered: pageStack.pushDialogLayer(Qt.createComponent("org.kde.plasmatube", "NetworkProxyPage"))
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    BaseGridView {
        id: gridView

        placeholderIconName: "videoclip-amarok"
        placeholderText: i18nc("@info:placeholder", "No Videos")
        model: VideoListModel {
            id: videoModel
        }
    }

    Component.onCompleted: gridView.model?.requestQuery(initialQuery, true)
}

