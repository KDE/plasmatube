// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-or-later

import org.kde.kirigami as Kirigami

import org.kde.plasmatube

FeedPage {
    actions: [
        Kirigami.Action {
            icon.name: "rss-symbolic"
            text: i18nc("@action:button View subscribed channels", "Subscribed Channels")
            onTriggered: pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "SubscriptionListPage"), { source: PlasmaTube.sourceManager.selectedSource })
        }
    ]

    initialQuery: VideoListModel.Feed
}