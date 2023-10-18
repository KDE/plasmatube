// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.8
import org.kde.plasmatube 1.0

Action {
    required property VideoListModel videoModel
    required property int queryType

    text: videoModel ? videoModel.queryTypeString(queryType) : ""
    icon.name: videoModel ? videoModel.queryTypeIcon(queryType) : ""
    onTriggered: {
        videoModel.requestQuery(queryType)
    }
}
