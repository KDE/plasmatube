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

    title: i18nc("@title:window", "Add PeerTube Source")

    function addSource(url) {
        let source = PlasmaTube.sourceManager.createPeerTubeSource(url);
    }

    FormCard.FormHeader {
        title: i18n("Select a instance")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: searchDelegate
            label: i18n("Search")
            placeholderText: i18n("joinpeertube.org")
        }

        FormCard.FormDelegateSeparator {}

        Repeater {
            id: instanceRepeater

            model: PeerTubeInstancesModel {
                filterString: searchDelegate.text
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
            visible: instanceRepeater.count === 0
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
