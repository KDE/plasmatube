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

    title: i18nc("@title:window", "Add PeerTube Source")

    function addSource(url) {
        PlasmaTube.sourceManager.createPeerTubeSource(url);
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
            placeholderText: i18n("joinpeertube.org")
        }

        FormCard.FormDelegateSeparator {}

        Repeater {
            id: instanceRepeater

            model: PeerTubeInstancesModel {
                filterString: searchDelegate.text
            }

            delegate: ColumnLayout {
                id: delegate

                required property int index
                required property string url

                FormCard.FormDelegateSeparator {
                    visible: delegate.index !== 0
                }

                FormCard.FormButtonDelegate {
                    text: delegate.url
                    onClicked: root.addSource(delegate.url)
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
