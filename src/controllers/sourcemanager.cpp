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

bool SourceManager::canRemove() const
{
    return m_sources.size() != 1;
}

void SourceManager::removeSource(VideoSource *source)
{
    source->logOut();

    auto config = KSharedConfig::openStateConfig();
    config->deleteGroup(QStringLiteral("source-%1").arg(source->uuid()));
    config->sync();

    const int row = static_cast<int>(m_sources.indexOf(source));
    beginRemoveRows(QModelIndex(), row, row);
    m_sources.removeAll(source);
    endRemoveRows();
}

bool SourceManager::hasAnySources() const
{
    return !m_sources.isEmpty();
}

bool SourceManager::hasFinishedLoading() const
{
    return m_finishedLoading;
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

void SourceManager::createPipedSource(const QString &url)
{
    auto source = new VideoSource(QUuid::createUuid().toString(), this);
    source->setType(VideoSource::Type::Piped);
    source->setUrl(url);

    insertSource(source);
}

void SourceManager::insertSource(VideoSource *pSource)
{
    const int index = m_finishedSources.size();

    beginInsertRows(QModelIndex(), static_cast<int>(m_sources.size()), static_cast<int>(m_sources.size()));
    m_sources.append(pSource);
    m_finishedSources.append(false);
    endInsertRows();
    Q_EMIT sourcesChanged();

    if (pSource->hasFinishedLoading()) {
        m_finishedSources[index] = true;
        checkIfFinishedLoading();
    } else {
        connect(pSource, &VideoSource::finishedLoading, this, [this, index] {
            m_finishedSources[index] = true;
            checkIfFinishedLoading();
        });
    }

    if (m_selectedSource == nullptr) {
        selectSource(pSource);
    }
}

void SourceManager::checkIfFinishedLoading()
{
    const bool didFinish = std::all_of(m_finishedSources.cbegin(), m_finishedSources.cend(), [](bool b) {
        return b == true;
    });

    if (didFinish != m_finishedLoading) {
        m_finishedLoading = didFinish;
        Q_EMIT finishedLoading();
    }
}

#include "moc_sourcemanager.cpp"