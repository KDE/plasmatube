// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractListModel>
#include "qinvidious/videobasicinfo.h"
#include "qinvidious/invidiousapi.h"

class InvidiousManager;
class QNetworkReply;

class VideoListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

public:
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
        PremiumRole
    };

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
    };
    Q_ENUM(QueryType)

    Q_INVOKABLE static QString queryTypeString(QueryType);
    Q_INVOKABLE static QString queryTypeIcon(QueryType);

    VideoListModel(QObject *parent = nullptr);
    VideoListModel(const QList<QInvidious::VideoBasicInfo> &, QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void fetchMore(const QModelIndex &parent) override;
    Q_INVOKABLE bool canFetchMore(const QModelIndex &parent) const override;

    Q_INVOKABLE void requestSearchResults(const QString &searchString);
    Q_INVOKABLE void requestQuery(QueryType type);
    Q_INVOKABLE void refresh();

    bool isLoading() const;
    Q_SIGNAL void isLoadingChanged();

    QString title() const;
    Q_SIGNAL void titleChanged();

    Q_SIGNAL void errorOccured(const QString &error);

private:
    void handleQuery(QFuture<QInvidious::VideoListResult> future, QueryType, bool reset = true);

    void setQueryType(QueryType);
    void clearAll();

    bool m_constant = false;

    QueryType m_queryType = NoQuery;
    qint32 m_currentPage = 0;
    QString m_searchQuery;
    QFutureWatcher<QInvidious::VideoListResult> *m_futureWatcher = nullptr;

    QList<QInvidious::VideoBasicInfo> m_results;
};

#endif // SEARCHMODEL_H
