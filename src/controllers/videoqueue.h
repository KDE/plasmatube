// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

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

    enum CustomRoles {
        VideoIdRole = Qt::UserRole,
    };

    [[nodiscard]] int rowCount(const QModelIndex &index) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void currentVideoChanged();

private:
    QList<QString> m_videoIds;
    int m_currentIndex = 0;
};
