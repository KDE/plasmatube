// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "invidiousinstancesmodel.h"

#include "plasmatube.h"

using namespace Qt::Literals::StringLiterals;

InvidiousInstancesModel::InvidiousInstancesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fill();
}

QVariant InvidiousInstancesModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &instance = m_instances[index.row()];

    switch (role) {
    case URLRole:
        return instance.url;
    default:
        return {};
    }
}

bool InvidiousInstancesModel::loading() const
{
    return m_loading;
}

void InvidiousInstancesModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int InvidiousInstancesModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_instances.size();
}

QHash<int, QByteArray> InvidiousInstancesModel::roleNames() const
{
    return {{URLRole, "url"}};
}

void InvidiousInstancesModel::fill()
{
    if (m_loading) {
        return;
    }
    setLoading(true);

    auto reply = m_netManager.get(QNetworkRequest(QUrl(QStringLiteral("https://api.invidious.io/instances.json"))));
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        auto instances = doc.array().toVariantList();

        if (!instances.isEmpty()) {
            QList<InvidiousInstance> fetchedInstances;

            std::transform(instances.cbegin(), instances.cend(), std::back_inserter(fetchedInstances), [this](const QVariant &value) -> auto {
                return fromSourceData(value.toJsonArray());
            });
            beginInsertRows({}, m_instances.size(), m_instances.size() + fetchedInstances.size() - 1);
            m_instances += fetchedInstances;
            endInsertRows();
        }

        setLoading(false);
    });
}

InvidiousInstancesModel::InvidiousInstance InvidiousInstancesModel::fromSourceData(const QJsonArray &array) const
{
    InvidiousInstance instance;
    instance.url = array[0].toString();

    return instance;
}

#include "moc_invidiousinstancesmodel.cpp"