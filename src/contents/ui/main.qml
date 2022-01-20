// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.19 as Kirigami

import org.kde.plasmatube 1.0

Kirigami.ApplicationWindow {
    id: root
    pageStack.initialPage: trendingPageComponent

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton

    contextDrawer: Kirigami.ContextDrawer {}
    
    footer: Kirigami.NavigationTabBar {
        id: navbar
        property bool shouldShow: pageStack.currentIndex === 0
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
            NumberAnimation {
                target: navbar
                property: "height"
                to: navbar.implicitHeight
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                target: navbar
                property: "opacity"
                to: 1
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
        
        SequentialAnimation {
            id: hideAnim
            NumberAnimation {
                target: navbar
                property: "opacity"
                to: 0
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                target: navbar
                property: "height"
                to: 0
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
        
        actions: [
            Kirigami.Action {
                iconName: "videoclip-amarok"
                text: i18n("Videos")
                checked: true // initial page that opens
                onTriggered: {
                    while (applicationWindow().pageStack.depth > 0) {
                        applicationWindow().pageStack.pop();
                    }
                    applicationWindow().pageStack.push(trendingPageComponent);
                }
            },
            Kirigami.Action {
                iconName: "search"
                text: i18n("Search")
                onTriggered: {
                    while (applicationWindow().pageStack.depth > 0) {
                        applicationWindow().pageStack.pop();
                    }
                    applicationWindow().pageStack.push(searchPageComponent);
                }
            },
            Kirigami.Action {
                iconName: "user-identity"
                text: i18n("Account")
                onTriggered: {
                    while (applicationWindow().pageStack.depth > 0) {
                        applicationWindow().pageStack.pop();
                    }
                    applicationWindow().pageStack.push(accountPageComponent);
                }
            }
        ]
    }

    Component {
        id: trendingPageComponent
        TrendingPage {}
    }
    Component {
        id: searchPageComponent
        SearchPage {}
    }
    Component {
        id: videoPageComponent
        VideoPage {}
    }
    Component {
        id: accountPageComponent
        AccountPage {}
    }
}
