// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractinstancesmodel.h"

#include <QNetworkAccessManager>
#include <QtQml>

class PipedInstancesModel : public AbstractInstancesModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit PipedInstancesModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;

Q_SIGNALS:
    void filterStringChanged();

private:
    void fill();

    struct PipedInstance {
        QString name, url, regions;
    };

    QList<PipedInstance> m_instances;
    QString m_filterString;
    QNetworkAccessManager m_netManager;
};
