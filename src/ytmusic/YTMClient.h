/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef YTMCLIENT_H
#define YTMCLIENT_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class YTMSearchResult;
class YTMApiRequest;

class YTMClient : public QObject
{
    Q_OBJECT

public:
    YTMClient(QObject *parent = nullptr);
    ~YTMClient();

    static YTMClient *instance();
    static QNetworkAccessManager *netManager();

    Q_INVOKABLE void fetchApiKey();
    
    QNetworkReply *sendRequest(const YTMApiRequest &request);

    bool hasFetchedApiKey() const;

signals:
    void apiKeyFetched(bool success);
    void searchResultReceived(const YTMSearchResult &result);

private:
    QNetworkAccessManager *m_netManager;
    QString m_apiKey;
};

#endif // YTMCLIENT_H
