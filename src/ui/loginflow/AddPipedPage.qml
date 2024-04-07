// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube

FormCard.FormCardPage {
    id: root

    title: i18nc("@title:window", "Add Piped Source")

    function addSource(url) {
        PlasmaTube.sourceManager.createPipedSource(url);
        if (root.Window.window !== applicationWindow()) {
            root.Window.window.close();
        }
    }

    FormCard.FormHeader {
        title: i18n("Select a instance")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: searchDelegate
            label: i18n("Search")
            placeholderText: i18n("pipedapi.kavin.rocks")
        }

        FormCard.FormDelegateSeparator {}

        Repeater {
            model: KItemModels.KSortFilterProxyModel {
                id: filterModel

                sourceModel: PipedInstancesModel {}

                filterString: searchDelegate.text
                filterRoleName: "url"
            }

            delegate: ColumnLayout {
                id: delegate

                required property int index
                required property string name
                required property string url

                FormCard.FormDelegateSeparator {
                    visible: delegate.index !== 0
                }

                FormCard.FormButtonDelegate {
                    text: delegate.name
                    onClicked: root.addSource(delegate.url)
                }
            }
        }

        FormCard.FormTextDelegate {
            text: i18n("No public instances found.")
            visible: filterModel.count === 0
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: i18nc("Add instance", "Add %1", searchDelegate.text)
            visible: searchDelegate.text.length !== 0
            onClicked: root.addSource(searchDelegate.text)
        }
    }
}
