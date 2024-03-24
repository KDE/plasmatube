// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.plasmatube
import org.kde.plasmatube.private

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
            checked: currentType === 0
            enabled: !Settings.isProxyTypeImmutable
            onToggled: currentType = 0
        }

        FormCard.FormDelegateSeparator { below: systemDefault; above: http }

        FormCard.FormRadioDelegate {
            id: http
            text: i18n("HTTP")
            checked: currentType === 1
            enabled: !Settings.isProxyTypeImmutable
            onToggled: currentType = 1
        }
    }

    FormCard.FormHeader {
        title: i18n("Proxy Settings")
    }

    FormCard.FormCard {
        enabled: currentType !== 0

        FormCard.FormTextFieldDelegate {
            id: hostField
            label: i18n("Host")
            text: Settings.proxyHost
            inputMethodHints: Qt.ImhUrlCharactersOnly
            onEditingFinished: proxyConfigChanged = true
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
                    value: Settings.proxyPort
                    from: 0
                    to: 65536
                    validator: IntValidator {bottom: portField.from; top: portField.to}
                    textFromValue: function(value, locale) {
                        return value; // it will add a thousands separator if we don't do this, not sure why
                    }
                    onValueChanged: proxyConfigChanged = true
                }
            }
        }
        FormCard.FormDelegateSeparator { below: portField; above: userField }
        FormCard.FormTextFieldDelegate {
            id: userField
            label: i18n("User")
            text: Settings.proxyUser
            inputMethodHints: Qt.ImhUrlCharactersOnly
            onEditingFinished: proxySettingsChanged = true
        }
        FormCard.FormDelegateSeparator { below: userField; above: passwordField }
        FormCard.FormTextFieldDelegate {
            id: passwordField
            label: i18n("Password")
            text: Settings.proxyPassword
            echoMode: TextInput.Password
            inputMethodHints: Qt.ImhUrlCharactersOnly
            onEditingFinished: proxyConfigChanged = true
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
                enabled: currentType !== Settings.proxyType || proxyConfigChanged
                onClicked: {
                    Settings.proxyType = currentType;
                    Settings.proxyHost = hostField.text;
                    Settings.proxyPort = portField.value;
                    Settings.proxyUser = userField.text;
                    Settings.proxyPassword = passwordField.text;
                    Settings.save();
                    proxyConfigChanged = false;
                    PlasmaTube.setApplicationProxy();
                }
            }
        }
    }

    Component.onCompleted: currentType = Settings.proxyType
}
