// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube

FormCard.FormCardPage {
    id: root

    title: i18nc("@title:window", "Network Proxy")

    property int currentType
    property bool proxyConfigChanged: false

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormRadioDelegate {
            id: systemDefault
            text: i18n("System Default")
            checked: root.currentType === 0
            enabled: !PlasmaTube.settings.isProxyTypeImmutable
            onToggled: root.currentType = 0
        }

        FormCard.FormDelegateSeparator { below: systemDefault; above: http }

        FormCard.FormRadioDelegate {
            id: http
            text: i18n("HTTP")
            checked: root.currentType === 1
            enabled: !PlasmaTube.settings.isProxyTypeImmutable
            onToggled: root.currentType = 1
        }
    }

    FormCard.FormHeader {
        title: i18n("Proxy Settings")
    }

    FormCard.FormCard {
        enabled: root.currentType !== 0

        FormCard.FormTextFieldDelegate {
            id: hostField
            label: i18n("Host")
            text: PlasmaTube.settings.proxyHost
            inputMethodHints: Qt.ImhUrlCharactersOnly
            onEditingFinished: root.proxyConfigChanged = true
        }
        FormCard.FormDelegateSeparator { below: hostField; above: portField }
        // we probably still need a FormSpinBoxDelegate
        FormCard.AbstractFormDelegate {
            Layout.fillWidth: true
            contentItem: RowLayout {
                QQC2.Label {
                    text: i18n("Port")
                    Layout.fillWidth: true
                }
                QQC2.SpinBox {
                    id: portField
                    value: PlasmaTube.settings.proxyPort
                    from: 0
                    to: 65536
                    validator: IntValidator {bottom: portField.from; top: portField.to}
                    textFromValue: function(value, locale) {
                        return value; // it will add a thousands separator if we don't do this, not sure why
                    }
                    onValueChanged: root.proxyConfigChanged = true
                }
            }
        }
        FormCard.FormDelegateSeparator { below: portField; above: userField }
        FormCard.FormTextFieldDelegate {
            id: userField
            label: i18n("User")
            text: PlasmaTube.settings.proxyUser
            inputMethodHints: Qt.ImhUrlCharactersOnly
            onEditingFinished: root.proxySettingsChanged = true
        }
        FormCard.FormDelegateSeparator { below: userField; above: passwordField }
        FormCard.FormTextFieldDelegate {
            id: passwordField
            label: i18n("Password")
            text: PlasmaTube.settings.proxyPassword
            echoMode: TextInput.Password
            inputMethodHints: Qt.ImhUrlCharactersOnly
            onEditingFinished: root.proxyConfigChanged = true
        }
    }

    footer: QQC2.ToolBar {
        height: visible ? implicitHeight : 0
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }

            QQC2.Button  {
                text: i18n("Apply")
                icon.name: "dialog-ok-apply-symbolic"
                enabled: root.currentType !== PlasmaTube.settings.proxyType || proxyConfigChanged
                onClicked: {
                    PlasmaTube.settings.proxyType = currentType;
                    PlasmaTube.settings.proxyHost = hostField.text;
                    PlasmaTube.settings.proxyPort = portField.value;
                    PlasmaTube.settings.proxyUser = userField.text;
                    PlasmaTube.settings.proxyPassword = passwordField.text;
                    PlasmaTube.settings.save();
                    proxyConfigChanged = false;
                    PlasmaTube.setApplicationProxy();
                }
            }
        }
    }

    Component.onCompleted: {
        proxyConfigChanged = false; // Make doubly sure that stupid bindings haven't turned this on
        currentType = PlasmaTube.settings.proxyType;
    }
}
