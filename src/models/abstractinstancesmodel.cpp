// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstractinstancesmodel.h"

#include "plasmatube.h"

#include <QtConcurrent>

AbstractInstancesModel::AbstractInstancesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> AbstractInstancesModel::roleNames() const
{
    return {{NameRole, "name"}, {URLRole, "url"}, {DescriptionRole, "description"}, {IconRole, "iconSource"}, {IsPublicRole, "isPublic"}};
}

bool AbstractInstancesModel::loading() const
{
    return m_loading;
}

void AbstractInstancesModel::setLoading(const bool loading)
{
    if (loading != m_loading) {
        m_loading = loading;
        Q_EMIT loadingChanged();
    }
}

#include "moc_abstractinstancesmodel.cpp"