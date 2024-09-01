// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kitemmodels as KItemModels

import org.kde.plasmatube

InstancesPage {
    id: root

    title: i18nc("@title:window", "Add Piped Source")

    model: KItemModels.KSortFilterProxyModel {
        id: filterModel

        sourceModel: PipedInstancesModel {}

        filterString: root.filterString
        filterRoleName: "url"
    }

    readonly property bool loading: model.sourceModel.loading

    function addSource(url) {
        PlasmaTube.sourceManager.createPipedSource(url);
        if (root.Window.window !== applicationWindow()) {
            root.Window.window.close();
        }
    }
}
