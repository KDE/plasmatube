// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sourcemanager.h"

#include <KSharedConfig>

SourceManager::SourceManager(QObject *parent)
    : QAbstractListModel(parent)
{
}

void SourceManager::load()
{
    auto config = KSharedConfig::openStateConfig();
    for (const auto &id : config->groupList()) {
        if (id.contains(QLatin1String("source-"))) {
            const QString uuid = QString(id).remove(QLatin1String("source-"));
            qInfo() << "Loading source" << uuid;

            auto source = new VideoSource(uuid, this);
            insertSource(source);
        }
    }

    // TODO: save last used source
    if (!m_sources.isEmpty()) {
        selectSource(m_sources[0]);
    }
}

int SourceManager::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return static_cast<int>(m_sources.size());
}

QVariant SourceManager::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index)) {
        return {};
    }

    const int row = index.row();
    if (role == SourceRole) {
        return QVariant::fromValue(m_sources[row]);
    }

    return {};
}

QHash<int, QByteArray> SourceManager::roleNames() const
{
    return {{SourceRole, QByteArrayLiteral("source")}};
}

void SourceManager::selectSource(VideoSource *source)
{
    if (!m_sources.contains(source)) {
        qDebug() << "WTF: attempt to select unmanaged source" << source;
        return;
    }

    m_selectedSource = source;

    Q_EMIT sourceSelected();
}

VideoSource *SourceManager::selectedSource() const
{
    return m_selectedSource;
}

int SourceManager::selectedIndex() const
{
    if (m_selectedSource != nullptr) {
        return m_sources.indexOf(m_selectedSource);
    } else {
        return -1;
    }
}

bool SourceManager::hasAnySources() const
{
    return !m_sources.isEmpty();
}

void SourceManager::createInvidiousSource(const QString &url)
{
    auto source = new VideoSource(QUuid::createUuid().toString(), this);
    source->setType(VideoSource::Type::Invidious);
    source->setUrl(url);

    insertSource(source);
}

void SourceManager::createPeerTubeSource(const QString &url)
{
    auto source = new VideoSource(QUuid::createUuid().toString(), this);
    source->setType(VideoSource::Type::PeerTube);
    source->setUrl(url);

    insertSource(source);
}

void SourceManager::insertSource(VideoSource *pSource)
{
    beginInsertRows(QModelIndex(), static_cast<int>(m_sources.size()), static_cast<int>(m_sources.size()));
    m_sources.append(pSource);
    endInsertRows();
    Q_EMIT sourcesChanged();

    if (m_selectedSource == nullptr) {
        selectSource(pSource);
    }
}
