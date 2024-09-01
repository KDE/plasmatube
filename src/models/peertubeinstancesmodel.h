// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractinstancesmodel.h"

#include <QNetworkAccessManager>
#include <QtQml>

class PeerTubeInstancesModel : public AbstractInstancesModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)

public:
    enum CustomRoles { NameRole = Qt::UserRole, URLRole, DescriptionRole, IconRole, IsPublicRole };

    explicit PeerTubeInstancesModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;

    void setFilterString(const QString &filterString);
    QString filterString() const;

Q_SIGNALS:
    void filterStringChanged();

private:
    void fill();

    struct PeerTubeInstance {
        QString name;
        QString url;
        QString shortDescription;
        QString avatarUrl;
    };

    QList<PeerTubeInstance> m_instances;
    QString m_filterString;
    PeerTubeInstance fromSourceData(const QJsonObject &object) const;
    QNetworkAccessManager m_netManager;
};
