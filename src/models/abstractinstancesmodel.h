// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtQml>

class QNetworkReply;

class AbstractInstancesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum Roles : int {
        // Common roles
        NameRole = Qt::UserRole,
        URLRole,
        DescriptionRole,
        IconRole,
        IsPublicRole,

        // Base role for sub-class roles.
        ExtraRole,
    };

    explicit AbstractInstancesModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;

    bool loading() const;

Q_SIGNALS:
    void loadingChanged();

protected:
    void setLoading(bool loading);

private:
    bool m_loading = false;
};
