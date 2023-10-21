// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

#include "videosource.h"

class SourceManager : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Use PlasmaTube.sourceManager")

    Q_PROPERTY(bool hasAnySources READ hasAnySources NOTIFY sourcesChanged)
    Q_PROPERTY(VideoSource *selectedSource READ selectedSource WRITE selectSource NOTIFY sourceSelected)
    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY sourceSelected)

public:
    explicit SourceManager(QObject *parent = nullptr);

    void load();

    enum CustomRoles {
        SourceRole = Qt::UserRole,
    };

    [[nodiscard]] int rowCount(const QModelIndex &index) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void selectSource(VideoSource *source);
    VideoSource *selectedSource() const;
    int selectedIndex() const;

    bool hasAnySources() const;

    Q_INVOKABLE void createInvidiousSource(const QString &url);
    Q_INVOKABLE void createPeerTubeSource(const QString &url);

Q_SIGNALS:
    void sourcesChanged();
    void sourceSelected();

private:
    QVector<VideoSource *> m_sources;
    void insertSource(VideoSource *pSource);
    VideoSource *m_selectedSource = nullptr;
};
