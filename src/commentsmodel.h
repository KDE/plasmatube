// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QFutureWatcher>
#include <QUrl>

#include "comment.h"
#include "invidiousapi.h"

class CommentsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum CustomRoles { AuthorRole = Qt::UserRole, AuthorAvatarRole, ContentRole };

    explicit CommentsModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void fillComments(const QString &videoId);
    Q_INVOKABLE void loadMore();

Q_SIGNALS:
    void loadingChanged();

private:
    void fill();

    QFutureWatcher<QInvidious::CommentsResult> *m_futureWatcher = nullptr;
    QList<QInvidious::Comment> m_comments;
    bool m_loading = false;
    QString m_continuation;
    QString m_videoId;
};
