// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

import org.kde.kirigami as Kirigami

import org.kde.plasmatube

Kirigami.Action {
    required property VideoListModel videoModel
    required property int queryType

    text: videoModel ? videoModel.queryTypeString(queryType) : ""
    icon.name: videoModel ? videoModel.queryTypeIcon(queryType) : ""
    onTriggered: {
        videoModel.requestQuery(queryType, true)
    }
}
