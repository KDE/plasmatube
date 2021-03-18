// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INVIDIOUSMANAGER_H
#define INVIDIOUSMANAGER_H

#include <QObject>
#include "video.h"

class QNetworkReply;
class QNetworkAccessManager;

class InvidiousManager : public QObject
{
    Q_OBJECT

public:
    enum VideoListType {
        Search,
        Trending,
        Top,
        Feed
    };
    Q_ENUM(VideoListType)

    explicit InvidiousManager(QObject *parent = nullptr);

    QString region() const;
    void setRegion(const QString&);

signals:
    void videoQueryResults(const QList<VideoBasicInfo>&);
    void videoQueryFailed(const QString &errorText);

    void videoReceived(const QJsonObject&);
    void videoRequestFailed();

public slots:
    QNetworkReply* videoQuery(VideoListType queryType, const QString &queryValue = QString(), qint32 page = 0);

    QNetworkReply* requestVideo(const QString& videoId);

private:
    QString invidiousInstance();
    QNetworkAccessManager *netManager();

    QString m_region;
};

#endif // INVIDIOUSMANAGER_H
