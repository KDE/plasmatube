// SPDX-FileCopyrightText: 2022 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigamiaddons.settings as KirigamiSettings

KirigamiSettings.CategorizedSettings {
    id: settingsPage

    actions: [
        KirigamiSettings.SettingAction {
            actionName: "general"
            text: i18n("General")
            icon.name: "org.kde.plasmatube"
            page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/plasmatube/ui/settings/GeneralPage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "sources"
            text: i18n("Sources")
            icon.name: "cloudstatus"
            page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/plasmatube/ui/settings/SourcesPage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "proxy"
            text: i18n("Network Proxy")
            icon.name: "network-connect"
            page: Qt.resolvedUrl("qrc:/qt/qml/org/kde/plasmatube/ui/settings/NetworkProxyPage.qml")
        },
        KirigamiSettings.SettingAction {
            actionName: "about"
            text: i18n("About PlasmaTube")
            icon.name: "help-about"
            onTriggered: pageStack.push(Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage"))
        },
        KirigamiSettings.SettingAction {
            actionName: "aboutkde"
            text: i18n("About KDE")
            icon.name: "kde"
            onTriggered: pageStack.push(Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutKDE"))
        }
    ]
}
