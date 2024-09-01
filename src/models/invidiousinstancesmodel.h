// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QtQml>

class InvidiousInstancesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum CustomRoles { NameRole = Qt::UserRole, URLRole, DescriptionRole, IconRole, IsPublicRole };

    explicit InvidiousInstancesModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void loadingChanged();

private:
    void fill();

    struct InvidiousInstance {
        QString name;
        QString url;
        QString region;
    };

    QList<InvidiousInstance> m_instances;
    bool m_loading = false;
    InvidiousInstance fromSourceData(const QJsonArray &object) const;
    QNetworkAccessManager m_netManager;
};
