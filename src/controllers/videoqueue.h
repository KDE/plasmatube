// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QFutureWatcher>
#include <QtQml/qqmlregistration.h>

#include "abstractapi.h"
#include "videobasicinfo.h"

class VideoQueue : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Use PlasmaTube.videoController")

public:
    explicit VideoQueue(QObject *parent = nullptr);

    void replace(const QStringList &videoIds);
    void queueNext(const QString &videoId);

    void next();
    void previous();

    QString getCurrentVideoId() const;

    // TODO: we should have an AbstractVideoListModel, and combine these roles with VideoLisTModel
    enum Roles : int {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ThumbnailRole,
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
        WatchedRole
    };

    [[nodiscard]] int rowCount(const QModelIndex &index) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void currentVideoChanged();

private:
    void requestMissingVideoInformation();

    QList<QString> m_videoIds;
    QList<std::optional<QInvidious::VideoBasicInfo>> m_videoInfo;
    int m_currentIndex = 0;
    QFutureWatcher<QInvidious::VideoResult> *m_watcher = nullptr;
};
