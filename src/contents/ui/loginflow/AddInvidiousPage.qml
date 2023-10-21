// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kitemmodels as KItemModels
import org.kde.plasmatube

FormCard.FormCardPage {
    id: root

    title: i18nc("@title:window", "Add Invidious Source")

    function addSource(url) {
        let source = PlasmaTube.sourceManager.createInvidiousSource(url);
    }

    FormCard.FormHeader {
        title: i18n("Select a instance")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: searchDelegate
            label: i18n("Search")
            placeholderText: i18n("invidious.io")
        }

        FormCard.FormDelegateSeparator {}

        Repeater {
            model: KItemModels.KSortFilterProxyModel {
                id: filterModel

                sourceModel: InvidiousInstancesModel {}

                filterString: searchDelegate.text
                filterRoleName: "url"
            }

            delegate: ColumnLayout {
                required property int index
                required property string url

                FormCard.FormDelegateSeparator {
                    visible: index !== 0
                }

                FormCard.FormButtonDelegate {
                    text: url
                    onClicked: root.addSource(url)
                }
            }
        }

        FormCard.FormTextDelegate {
            text: i18n("No public instances found.")
            visible: filterModel.count === 0
        }

        FormCard.FormDelegateSeparator {
            visible: searchDelegate.text.length !== 0
        }

        FormCard.FormButtonDelegate {
            text: searchDelegate.text
            visible: searchDelegate.text.length !== 0
            onClicked: root.addSource(searchDelegate.text)
        }
    }
}
