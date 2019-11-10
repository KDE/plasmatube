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

#include "YTMClient.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include "constants.h"
#include "YTMSearchResult.h"
#include "YTMSearchRequest.h"

#define YTMUSIC_SUPPORTED_USER_AGENT "Mozilla/5.0 (X11; Linux x86_64; rv:72.0) Gecko/20100101 Firefox/72.0"

#define HTTP_USER_AGENT "User-Agent"

#define API_KEY_SEARCH_TERM R"("INNERTUBE_API_KEY":")"
#define API_KEY_END_SEARCH_TEAM '"'

// extract INNERTUBE_API_KEY from HTML/JS
static const QString extractApiKeyFromHtml(const QByteArray &html)
{
    const auto searchString = QByteArrayLiteral(API_KEY_SEARCH_TERM);
    int apiKeyIndex = html.indexOf(searchString);
    if (apiKeyIndex < 0) {
        qWarning() << "Could not extract YouTube Music API key: website does not contain"
                   << QLatin1String(API_KEY_SEARCH_TERM);
        return {};
    }

    apiKeyIndex += searchString.size();
    int apiKeyEndIndex = html.indexOf(API_KEY_END_SEARCH_TEAM, apiKeyIndex);
    if (apiKeyEndIndex < 0) {
        qWarning() << "Could not extract YouTube Music API key: recognized key has no end marked with an '\"'";
        return {};
    }

    const QString apiKey = QString::fromUtf8(html.mid(apiKeyIndex, apiKeyEndIndex - apiKeyIndex));

    // the API key should usually have a length of 39 bytes
    // if the key is much longer than this, the result is probably wrong
    if (apiKey.size() < 25 || apiKey.size() > 100) {
        qWarning() << "Could not extract YouTube Music API key from HTML!\n"
                   << "The extracted result has a length of"
                   << apiKey.size()
                   << "characters - This is probably not the API key.";
        return {};
    }

    return apiKey;
}

YTMClient *s_instance = nullptr;

YTMClient::YTMClient(QObject *parent)
    : QObject(parent),
      m_netManager(new QNetworkAccessManager(this))
{
    Q_ASSERT(!s_instance);
    s_instance = this;
}

YTMClient::~YTMClient()
{
    s_instance = nullptr;
}

YTMClient *YTMClient::instance()
{
    if (!s_instance)
        s_instance = new YTMClient();
    return s_instance;
}

QNetworkAccessManager *YTMClient::netManager()
{
    return YTMClient::instance()->m_netManager;
}

void YTMClient::fetchApiKey()
{
    QNetworkRequest request(QStringLiteral(YTMUSIC_URL));
    request.setRawHeader(HTTP_USER_AGENT, YTMUSIC_SUPPORTED_USER_AGENT);

    QNetworkReply *reply = m_netManager->get(request);

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        m_apiKey = extractApiKeyFromHtml(reply->readAll());

        emit apiKeyFetched(!m_apiKey.isEmpty());

        reply->deleteLater();
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError) {
        reply->deleteLater();
    });
}

QNetworkReply *YTMClient::sendRequest(const YTMApiRequest &request)
{
    QNetworkRequest netRequest(QStringLiteral(YTMUSIC_URL).append(request.urlPath(m_apiKey)));
    request.setHeaders(netRequest);
    
    qDebug() << request.toByteArray();

    return m_netManager->post(netRequest, request.toByteArray());
}

bool YTMClient::hasFetchedApiKey() const
{
    return !m_apiKey.isEmpty();
}
