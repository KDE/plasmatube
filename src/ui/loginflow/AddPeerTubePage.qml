// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube

InstancesPage {
    id: root

    title: i18nc("@title:window", "Add PeerTube Source")

    model: PeerTubeInstancesModel {
        filterString: root.filterString
    }

    readonly property bool loading: model.loading

    function addSource(url) {
        PlasmaTube.sourceManager.createPeerTubeSource(url);
        if (root.Window.window !== applicationWindow()) {
            root.Window.window.close();
        }
    }
}
