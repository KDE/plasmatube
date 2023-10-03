// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami

Kirigami.NavigationTabBar {
    id: root

    property bool shouldShow: applicationWindow().pageStack.currentIndex === 0
    onShouldShowChanged: {
        if (shouldShow) {
            hideAnim.stop();
            showAnim.restart();
        } else {
            showAnim.stop();
            hideAnim.restart();
        }
    }

    visible: height !== 0

    // animate showing and hiding of navbar
    ParallelAnimation {
        id: showAnim
        onFinished: root.height = Qt.binding(() => root.implicitHeight);
        NumberAnimation {
            target: root
            property: "height"
            to: root.implicitHeight
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: root
            property: "opacity"
            to: 1
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    SequentialAnimation {
        id: hideAnim
        onFinished: root.height = Qt.binding(() => root.implicitHeight);
        NumberAnimation {
            target: root
            property: "opacity"
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: root
            property: "height"
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    property var pageStack: applicationWindow().pageStack

    actions: [
        Kirigami.Action {
            icon.name
    :
    "videoclip-amarok"
            text: i18n("Videos")
            checked: true // initial page that opens
            onTriggered: {
                while (applicationWindow().pageStack.depth > 0) {
                    applicationWindow().pageStack.pop();
                }
                applicationWindow().pageStack.push(applicationWindow().getPage("TrendingPage"));
                applicationWindow().closePlayer();
            }
        },
        Kirigami.Action {
            icon.name: "search"
            text: i18n("Search")
            onTriggered: {
                while (applicationWindow().pageStack.depth > 0) {
                    applicationWindow().pageStack.pop();
                }
                applicationWindow().pageStack.push(applicationWindow().getPage("SearchPage"));
                applicationWindow().closePlayer();
            }
        },
        Kirigami.Action {
            icon.name: "settings-configure"
            text: i18n("Settings")
            onTriggered: {
                while (applicationWindow().pageStack.depth > 0) {
                    applicationWindow().pageStack.pop();
                }
                applicationWindow().pageStack.push(applicationWindow().getPage("SettingsPage"));
                applicationWindow().closePlayer();
            }
        }
    ]
}

