// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videoqueue.h"

VideoQueue::VideoQueue(QObject *parent)
    : QAbstractListModel(parent)
{
}

void VideoQueue::replace(const QStringList &videoIds)
{
    beginResetModel();
    m_videoIds = videoIds;
    endResetModel();

    m_currentIndex = 0;
    Q_EMIT currentVideoChanged();
}

void VideoQueue::queueNext(const QString &videoId)
{
    beginInsertRows({}, m_videoIds.size(), m_videoIds.size());
    m_videoIds.push_back(videoId);
    endInsertRows();
}

void VideoQueue::next()
{
    if (m_currentIndex + 1 < m_videoIds.size()) {
        m_currentIndex++;
        Q_EMIT currentVideoChanged();
    }
}

void VideoQueue::previous()
{
    if (m_currentIndex - 1 > 0) {
        m_currentIndex--;
        Q_EMIT currentVideoChanged();
    }
}

QString VideoQueue::getCurrentVideoId() const
{
    if (m_videoIds.isEmpty()) {
        return QString();
    }
    return m_videoIds[m_currentIndex];
}

int VideoQueue::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return static_cast<int>(m_videoIds.size());
}

QVariant VideoQueue::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index)) {
        return {};
    }

    const int row = index.row();
    if (role == VideoIdRole) {
        return m_videoIds[row];
    }

    return {};
}

QHash<int, QByteArray> VideoQueue::roleNames() const
{
    return {{VideoIdRole, QByteArrayLiteral("videoId")}};
}

#include "moc_videoqueue.cpp"