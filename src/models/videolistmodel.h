// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractapi.h"
#include "abstractlistmodel.h"

#include <searchparameters.h>
#include <videobasicinfo.h>

#include <QAbstractListModel>
#include <QFutureSynchronizer>
#include <QtQml>

class InvidiousManager;
class QNetworkReply;

class VideoListModel : public AbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

public:
    enum QueryType : quint8 {
        NoQuery,
        Feed,
        Search,
        Top,
        Trending,
        TrendingGaming,
        TrendingMovies,
        TrendingMusic,
        TrendingNews,
        RecommendedVideos,
        Channel,
        History,
        Playlist
    };
    Q_ENUM(QueryType)

    Q_INVOKABLE static QString queryTypeString(QueryType);
    Q_INVOKABLE static QString queryTypeIcon(QueryType);

    explicit VideoListModel(QObject *parent = nullptr);
    explicit VideoListModel(const QList<QInvidious::VideoBasicInfo> &, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;

    QString title() const;

public Q_SLOTS:
    void requestChannel(const QString &ucid);
    void requestQuery(VideoListModel::QueryType type);
    void requestPlaylist(const QString &id);
    void refresh() override;
    void markAsWatched(int index) override;
    void markAsUnwatched(int index) override;
    void removeFromPlaylist(const QString &plid, int index) override;

Q_SIGNALS:
    void titleChanged();

private:
    void handleQuery(QFuture<QInvidious::VideoListResult> future, QueryType, bool reset = true);

    void setQueryType(QueryType);
    void clearAll();

    QueryType m_queryType = NoQuery;
    qint32 m_currentPage = 1;
    SearchParameters m_searchParameters;
    QString m_channel;
    QFutureWatcher<QInvidious::VideoListResult> *m_futureWatcher = nullptr;

    // history
    QFutureWatcher<QInvidious::HistoryResult> *m_historyPageWatcher = nullptr;
    QFutureSynchronizer<QInvidious::VideoResult> m_historyFutureSync;
    QFutureWatcher<void> *m_historyFetchFinishWatcher = nullptr;

    // playlist
    QString m_playlist;

    QList<QInvidious::VideoBasicInfo> m_results;

    void requestHistory();
    void processHistoryResult(const QList<QString> &result);
};
