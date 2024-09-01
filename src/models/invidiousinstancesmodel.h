// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractinstancesmodel.h"

#include <QNetworkAccessManager>
#include <QtQml>

class InvidiousInstancesModel : public AbstractInstancesModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit InvidiousInstancesModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;

private:
    void fill();

    struct InvidiousInstance {
        QString name;
        QString url;
        QString region;
    };

    QList<InvidiousInstance> m_instances;
    InvidiousInstance fromSourceData(const QJsonArray &object) const;
    QNetworkAccessManager m_netManager;
};
