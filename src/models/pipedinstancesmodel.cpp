// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pipedinstancesmodel.h"

#include "plasmatube.h"

using namespace Qt::Literals::StringLiterals;

PipedInstancesModel::PipedInstancesModel(QObject *parent)
    : AbstractInstancesModel(parent)
{
    fill();
}

QVariant PipedInstancesModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &instance = m_instances[index.row()];

    switch (role) {
    case NameRole:
        return instance.name;
    case URLRole:
        return instance.url;
    case DescriptionRole:
        return instance.regions;
    case IconRole:
        return QStringLiteral("plasmatube-piped");
    case IsPublicRole:
        return true;
    default:
        return {};
    }
}

int PipedInstancesModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_instances.size());
}

void PipedInstancesModel::fill()
{
    if (loading()) {
        return;
    }
    setLoading(true);

    QUrl url(QStringLiteral("https://raw.githubusercontent.com/wiki/TeamPiped/Piped/Instances.md"));

    auto reply = m_netManager.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        QTextStream stream(reply->readAll());

        bool beginList = false;

        QString line;
        while (stream.readLineInto(&line)) {
            if (line == QStringLiteral("--- | --- | --- | --- | ---")) {
                beginList = true;
            } else if (beginList) {
                const QStringList parts = line.split("|"_L1);
                const QString name = parts[0].trimmed();
                const QString url = parts[1].trimmed();
                const QString regions = parts[2].trimmed();

                beginInsertRows({}, m_instances.size(), m_instances.size());
                m_instances.push_back({name, url, regions});
                endInsertRows();
            }
        }

        setLoading(false);
    });
}

#include "moc_pipedinstancesmodel.cpp"