// SPDX-FileCopyrightText: 2022 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigamiaddons.settings as KirigamiSettings

KirigamiSettings.ConfigurationView {
    id: settingsPage

    modules: [
        KirigamiSettings.ConfigurationModule {
            moduleId: "general"
            text: i18n("General")
            icon.name: "org.kde.plasmatube"
            page: () => Qt.createComponent("org.kde.plasmatube", "GeneralPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "sources"
            text: i18n("Sources")
            icon.name: "cloudstatus"
            page: () => Qt.createComponent("org.kde.plasmatube", "SourcesPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "proxy"
            text: i18n("Network Proxy")
            icon.name: "network-connect"
            page: () => Qt.createComponent("org.kde.plasmatube", "NetworkProxyPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "about"
            text: i18n("About PlasmaTube")
            icon.name: "help-about"
            page: () => Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "aboutkde"
            text: i18n("About KDE")
            icon.name: "kde"
            page: () => Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutKDEPage")
        }
    ]
}
