// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

import org.kde.plasmatube

QQC2.Button {
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

        QQC2.BusyIndicator {
            visible: subscriptionController.isLoading
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2
            Layout.preferredWidth: Kirigami.Units.gridUnit * 2
        }

        QQC2.Label {
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