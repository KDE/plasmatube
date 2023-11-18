// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <abstractapi.h>
#include <searchparameters.h>
#include <videobasicinfo.h>

#include <QAbstractListModel>
#include <QFutureSynchronizer>
#include <QtQml>

class QNetworkReply;

class AbstractListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    enum Roles : int {
        // Common roles
        IdRole = Qt::UserRole + 1,
        TypeRole,
        TitleRole,
        ThumbnailRole,

        // Video roles
        LengthRole,
        ViewCountRole,
        AuthorRole,
        AuthorIdRole,
        AuthorUrlRole,
        PublishedRole,
        PublishedTextRole,
        DescriptionRole,
        DescriptionHtmlRole,
        LiveNowRole,
        PaidRole,
        PremiumRole,
        WatchedRole,

        // Playlist roles
        VideoCountRole,

        // Channel roles
        ChannelNameRole,
        ChannelAvatarRole
    };

    explicit AbstractListModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;

    bool isLoading() const;

public Q_SLOTS:
    virtual void refresh()
    {
    }

    virtual void markAsWatched(int index)
    {
        Q_UNUSED(index)
    }

    virtual void markAsUnwatched(int index)
    {
        Q_UNUSED(index)
    }

    virtual void removeFromPlaylist(const QString &plid, int index)
    {
        Q_UNUSED(plid)
        Q_UNUSED(index)
    }

Q_SIGNALS:
    void isLoadingChanged();
    void errorOccured(const QString &error);

protected:
    void setLoading(bool loading);

private:
    bool m_loading = false;
};
