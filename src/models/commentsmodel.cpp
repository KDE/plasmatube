// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "commentsmodel.h"

#include "plasmatube.h"

using namespace Qt::Literals::StringLiterals;

CommentsModel::CommentsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_paginator(this)
{
}

QVariant CommentsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &comment = m_comments[index.row()];

    switch (role) {
    case AuthorRole:
        return comment.author();
    case AuthorAvatarRole:
        return comment.authorAvatar();
    case ContentRole:
        return comment.content();
    default:
        return {};
    }
}

bool CommentsModel::loading() const
{
    return m_loading;
}

void CommentsModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int CommentsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_comments.size());
}

QHash<int, QByteArray> CommentsModel::roleNames() const
{
    return {
        {AuthorRole, "author"},
        {AuthorAvatarRole, "authorAvatar"},
        {ContentRole, "content"},
    };
}

void CommentsModel::fillComments(const QString &videoId)
{
    if (!m_comments.isEmpty()) {
        beginResetModel();
        m_comments.clear();
        endResetModel();
    }

    m_paginator.reset();
    m_videoId = videoId;
    fill();
}

void CommentsModel::loadMore()
{
    if (m_paginator.hasMore()) {
        m_paginator.next();
        fill();
    }
}

void CommentsModel::fill()
{
    if (m_loading) {
        return;
    }

    setLoading(true);

    auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestComments(m_videoId, &m_paginator);

    m_futureWatcher = new QFutureWatcher<QInvidious::CommentsResult>();
    m_futureWatcher->setFuture(future);

    connect(m_futureWatcher, &QFutureWatcherBase::finished, this, [this] {
        if (m_futureWatcher->future().resultCount() != 0) {
            auto result = m_futureWatcher->result();
            if (auto comments = std::get_if<QList<QInvidious::Comment>>(&result)) {
                const auto rows = rowCount({});
                beginInsertRows({}, rows, rows + static_cast<int>(comments->size()) - 1);
                m_comments << *comments;
                endInsertRows();
            } else if (auto error = std::get_if<QInvidious::Error>(&result)) {
                // TODO: Log error
            }
        }

        m_futureWatcher->deleteLater();
        m_futureWatcher = nullptr;
        setLoading(false);
    });
}

#include "moc_commentsmodel.cpp"