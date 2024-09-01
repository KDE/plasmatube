// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QtQml>

class PipedInstancesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum CustomRoles { NameRole = Qt::UserRole, URLRole, DescriptionRole, IconRole, IsPublicRole };

    explicit PipedInstancesModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void loadingChanged();
    void filterStringChanged();

private:
    void fill();

    struct PipedInstance {
        QString name, url, regions;
    };

    QList<PipedInstance> m_instances;
    bool m_loading = false;
    QString m_filterString;
    QNetworkAccessManager m_netManager;
};
