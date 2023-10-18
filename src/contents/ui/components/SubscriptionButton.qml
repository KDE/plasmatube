// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami
import QtMultimedia 5.15
import Qt5Compat.GraphicalEffects

import org.kde.plasmatube 1.0
import "../utils.js" as Utils

Controls.Button {
    id: subscribeButton
    Layout.topMargin: Kirigami.Units.smallSpacing

    property alias channelId: subscriptionController.channelId
    property string subCountText

    SubscriptionController {
        id: subscriptionController
    }

    background: Rectangle {
        color: subscriptionController.isSubscribed ? "grey" : "red"
        radius: Kirigami.Units.smallSpacing

        Rectangle {
            anchors.fill: parent
            radius: Kirigami.Units.smallSpacing
            color: "black"
            opacity: subscribeButton.pressed ? 0.6 : subscribeButton.hovered ? 0.3 : 0
        }
    }

    leftPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    topPadding: Kirigami.Units.largeSpacing

    contentItem: RowLayout {
        id: subscribeButtonContent
        spacing: 0

        Controls.BusyIndicator {
            visible: subscriptionController.isLoading
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2
            Layout.preferredWidth: Kirigami.Units.gridUnit * 2
        }

        Controls.Label {
            color: "white"
            text: {
                if (subscriptionController.isSubscribed) {
                    return i18n("Unsubscribe (%1)", subscribeButton.subCountText)
                }
                return i18n("Subscribe (%1)", subscribeButton.subCountText)
            }
        }
    }

    onClicked: {
        if (subscriptionController.canToggleSubscription()) {
            subscriptionController.toggleSubscription()
        } else if (!PlasmaTube.isLoggedIn) {
            root.requestClosePlayer();
            showPassiveNotification(i18n("Please log in to subscribe to channels."));
            pageStack.layers.push("qrc:/LoginPage.qml");
        }
    }
}