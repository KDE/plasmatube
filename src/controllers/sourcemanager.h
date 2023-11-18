// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "videosource.h"

#include <QAbstractListModel>
#include <QtQml>

class SourceManager : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Use PlasmaTube.sourceManager")

    Q_PROPERTY(bool hasAnySources READ hasAnySources NOTIFY sourcesChanged)
    Q_PROPERTY(bool finishedLoading READ hasFinishedLoading NOTIFY finishedLoading)
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

    Q_INVOKABLE bool canRemove() const;
    Q_INVOKABLE void removeSource(VideoSource *source);

    bool hasAnySources() const;
    bool hasFinishedLoading() const;

    Q_INVOKABLE void createInvidiousSource(const QString &url);
    Q_INVOKABLE void createPeerTubeSource(const QString &url);
    Q_INVOKABLE void createPipedSource(const QString &url);

Q_SIGNALS:
    void sourcesChanged();
    void sourceSelected();
    void finishedLoading();

private:
    QVector<VideoSource *> m_sources;
    QVector<bool> m_finishedSources;
    void insertSource(VideoSource *pSource);
    VideoSource *m_selectedSource = nullptr;
    bool m_finishedLoading = false;
    void checkIfFinishedLoading();
};
