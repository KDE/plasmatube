// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "commentsmodel.h"
#include "controllers/plasmatube.h"

using namespace Qt::Literals::StringLiterals;

CommentsModel::CommentsModel(QObject *parent)
    : QAbstractListModel(parent)
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
    return parent.isValid() ? 0 : m_comments.size();
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

    m_continuation.clear();
    m_videoId = videoId;
    fill();
}

void CommentsModel::loadMore()
{
    if (!m_continuation.isEmpty()) {
        fill();
    }
}

void CommentsModel::fill()
{
    if (m_loading) {
        return;
    }
    setLoading(true);

    m_futureWatcher = new QFutureWatcher<QInvidious::CommentsResult>();

    auto future = PlasmaTube::instance().sourceManager()->selectedSource()->api()->requestComments(m_videoId, m_continuation);
    m_futureWatcher->setFuture(future);

    connect(m_futureWatcher, &QFutureWatcherBase::finished, this, [this] {
        auto result = m_futureWatcher->result();
        if (auto comments = std::get_if<QInvidious::Comments>(&result)) {
            const auto rows = rowCount({});
            beginInsertRows({}, rows, rows + comments->comments.size() - 1);
            m_comments << (*comments).comments;
            endInsertRows();

            m_continuation = comments->continuation;
        } else if (auto error = std::get_if<QInvidious::Error>(&result)) {
            // TODO: Log error
        }

        m_futureWatcher->deleteLater();
        m_futureWatcher = nullptr;
        setLoading(false);
    });
}

#include "moc_commentsmodel.cpp"