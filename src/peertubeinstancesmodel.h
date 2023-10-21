// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class PeerTubeInstancesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)

public:
    enum CustomRoles { URLRole = Qt::UserRole };

    explicit PeerTubeInstancesModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setFilterString(const QString &filterString);
    QString filterString() const;

Q_SIGNALS:
    void loadingChanged();
    void filterStringChanged();

private:
    void fill();

    struct PeerTubeInstance {
        QString url;
    };

    QList<PeerTubeInstance> m_instances;
    bool m_loading = false;
    QString m_filterString;
    PeerTubeInstance fromSourceData(const QJsonObject &object) const;
    QNetworkAccessManager m_netManager;
};
