// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Window
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiComponents

import org.kde.plasmatube

Kirigami.ScrollablePage {
    id: root

    required property var source
    property bool importing: false

    title: i18nc("@title:window List of subscribed channels", "Subscribed Channels")

    Connections {
        target: source

        function onImportExportCompleted(numSubscriptions: int): void {
            // Prevents the progress dialog from being stuck open if this process happens too fast
            Qt.callLater(() => importExportProgressDialog.close());

            importExportDoneDialog.title = root.importing ? i18n("Importing Completed") : i18n("Exporting Completed");
            if (importing) {
                if (numSubscriptions > 0) {
                    importExportDoneDialog.subtitle = i18np("Imported %1 new subscription.", "Imported %1 new subscriptions.", numSubscriptions);
                } else {
                    importExportDoneDialog.subtitle = i18n("All imported channels were already subscribed to.")
                }
            } else {
                importExportDoneDialog.subtitle = i18np("Exported %1 subscription.", "Exported %1 subscriptions.", numSubscriptions);
            }
            importExportDoneDialog.open();
        }

        function onImportExportError(reason: string): void {
            // Prevents the progress dialog from being stuck open if this process happens too fast
            Qt.callLater(() => importExportProgressDialog.close());

            importExportDoneDialog.title = i18nc("@title:window Error while import/export", "Error");
            importExportDoneDialog.subtitle = reason;
            importExportDoneDialog.open();
        }
    }

    actions: [
        Kirigami.Action {
            text: i18nc("@action:button Import subscriptions", "Import…")
            icon.name: "document-import-symbolic"
            enabled: !listview.model.loading
            onTriggered: {
                fileDialog.fileMode = FileDialog.OpenFile;
                fileDialog.title = i18nc("@title:window Import the OPML file", "Import OPML Subscriptions");
                fileDialog.acceptLabel = i18nc("@action:button Import the OPML file", "Import");
                fileDialog.open();
            }
        },
        Kirigami.Action {
            text: i18nc("@action:button Export subscriptions", "Export…")
            icon.name: "document-import-symbolic"
            enabled: listview.count !== 0 && !listview.model.loading
            onTriggered: {
                fileDialog.fileMode = FileDialog.SaveFile;
                fileDialog.title = i18nc("@title:window Export the OPML file", "Export OPML Subscriptions");
                fileDialog.acceptLabel = i18nc("@action:button Export the OPML file", "Export");
                fileDialog.open();
            }
        }
    ]

    FileDialog {
        id: fileDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        defaultSuffix: "opml"
        onAccepted: {
            if (fileMode === FileDialog.OpenFile) {
                importing = true;
                root.source.importSubscriptions(selectedFile);
            } else {
                importing = false;
                root.source.exportSubscriptions(listview.model, selectedFile);
            }

            importExportProgressDialog.open();
        }
        visible: false
    }

    Kirigami.PromptDialog {
        id: importExportProgressDialog

        title: root.importing ? i18n("Importing…") : i18n("Exporting…")
        showCloseButton: false
        closePolicy: QQC2.Popup.NoAutoClose
        standardButtons: QQC2.Dialog.NoButton

        QQC2.ProgressBar {
            indeterminate: true

            Layout.fillWidth: true
        }
    }

    Kirigami.PromptDialog {
        id: importExportDoneDialog

        showCloseButton: false
    }

    ListView {
        id: listview

        model: SubscriptionListModel {}
        currentIndex: -1

        delegate: QQC2.ItemDelegate {
            id: delegate

            required property var index
            required property string name
            required property string channelId
            required property string avatar
            required property int subCount

            width: ListView.view.width

            onClicked: pageStack.layers.push(Qt.createComponent("org.kde.plasmatube", "ChannelPage"), {author: delegate.name, authorId: delegate.channelId})

            contentItem: RowLayout {
                spacing: Kirigami.Units.largeSpacing

                KirigamiComponents.AvatarButton {
                    Layout.preferredHeight: 50
                    Layout.preferredWidth: 50

                    name: delegate.name
                    source: delegate.avatar

                    onClicked: delegate.clicked()
                }

                QQC2.Label {
                    text: delegate.name
                    font.weight: Font.Bold
                }

                Item {
                    Layout.fillWidth: true
                }

                SubscriptionButton {
                    id: subscribeButton

                    channelId: delegate.channelId
                    subCount: delegate.subCount
                }
            }
        }

        Kirigami.LoadingPlaceholder {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No channels")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }

    Component.onCompleted: listview.model.fill(root.source)
}
