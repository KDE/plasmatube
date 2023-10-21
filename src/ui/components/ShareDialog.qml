// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.purpose as Purpose
import org.kde.notification
import org.kde.kirigami as Kirigami

Kirigami.Page {
    id: window

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None

    property alias index: jobView.index
    property alias model: jobView.model

    QQC2.Action {
        shortcut: 'Escape'
        onTriggered: window.closeDialog()
    }

    Notification {
        id: sharingFailed
        eventId: "sharingFailed"
        text: i18n("Sharing failed")
        urgency: Notification.NormalUrgency
    }

    Notification {
        id: sharingSuccess
        eventId: "sharingSuccess"
        flags: Notification.Persistent
    }

    Component.onCompleted: jobView.start()

    contentItem: Purpose.JobView {
        id: jobView
        onStateChanged: {
            if (state === Purpose.PurposeJobController.Finished) {
                window.closeDialog()
            } else if (state === Purpose.PurposeJobController.Error) {
                // Show failure notification
                sharingFailed.sendEvent();

                window.closeDialog()
            } else if (state === Purpose.PurposeJobController.Cancelled) {
                // Do nothing
                window.closeDialog()
            }
        }
    }
}