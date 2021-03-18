// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractListModel>
#include "videobasicinfo.h"
#include "invidiousmanager.h"

class InvidiousManager;
class QNetworkReply;

class VideoListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(InvidiousManager::VideoListType queryType READ queryType WRITE setQueryType NOTIFY queryTypeChanged)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

public:
    enum Roles {
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

    VideoListModel(QObject *parent = nullptr);
    VideoListModel(const QList<VideoBasicInfo>&, QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void fetch();
    Q_INVOKABLE void fetchMore(const QModelIndex &parent) override;
    Q_INVOKABLE bool canFetchMore(const QModelIndex &parent) const override;

    bool isLoading() const;

    QString query() const;
    void setQuery(const QString&);

    InvidiousManager::VideoListType queryType() const;
    void setQueryType(InvidiousManager::VideoListType queryType);

signals:
    void isLoadingChanged();
    void queryTypeChanged();
    void queryChanged();

private slots:
    void handleSearchResults(const QList<VideoBasicInfo>&);
    void handleSearchFailure();

private:
    void setIsLoading(bool);
    void clearAll();

    bool m_constant = false;
    bool m_loading = false;
    QString m_query;
    InvidiousManager::VideoListType m_queryType;
    qint32 m_nextPage = 0;

    QList<VideoBasicInfo> m_results;
    InvidiousManager *m_invidious;
    QNetworkReply *m_lastRequest = nullptr;
};

#endif // SEARCHMODEL_H
