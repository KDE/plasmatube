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

#include "youtubemusicmanager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include "YTMSearchResult.h"

#define YTMUSIC_URL "https://music.youtube.com"
#define YTMUSIC_API_SEARCH "/youtubei/v1/search"
#define YTMUSIC_SUPPORTED_USER_AGENT "Mozilla/5.0 (X11; Linux x86_64; rv:72.0) Gecko/20100101 Firefox/72.0"

#define HTTP_USER_AGENT "User-Agent"
#define CONTENTTYPE_JSON "application/json"

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

class SearchRequest
{
public:
    QByteArray toData(const QString &searchQuery) const;
};

QByteArray SearchRequest::toData(const QString &searchQuery) const
{
    QJsonObject clientObj;
    clientObj.insert("clientName", "WEB_REMIX");
    clientObj.insert("clientVersion", "0.1");
    clientObj.insert("hl", QLocale::system().name().split("_").at(0));
    clientObj.insert("gl", QLocale::system().name().split("_").at(1));
    clientObj.insert("experimentIds", QJsonArray());
    clientObj.insert("experimentsToken", QString());
    clientObj.insert("utcOffsetMinutes", 60);
    clientObj.insert("locationInfo", QJsonObject());
    clientObj.insert("musicAppInfo", QJsonObject());

    QJsonObject contextRequestObj;
    contextRequestObj.insert("internalExperimentFlags", QJsonObject());
    contextRequestObj.insert("sessionIndex", 0);

    QJsonObject contextObj;
    contextObj.insert("client", clientObj);
    contextObj.insert("capabilities", QJsonObject());
    contextObj.insert("request", QJsonObject());
    contextObj.insert("clickTracking", QJsonObject());
    contextObj.insert("user", QJsonObject());

    QJsonObject jsonObj;
    jsonObj.insert("context", contextObj);
    jsonObj.insert("query", searchQuery);
//     jsonObj.insert("params", "Eg-KAQwIABAAGAAgASgAMABqChAEEAMQCRAFEAo%3D");
    jsonObj.insert("params", "");

    QJsonDocument requestDoc;
    requestDoc.setObject(jsonObj);

    return requestDoc.toJson();
}

YouTubeMusicManager::YouTubeMusicManager(QObject *parent)
    : QObject(parent),
      m_netManager(new QNetworkAccessManager(this))
{
}

YouTubeMusicManager::~YouTubeMusicManager()
{
}

void YouTubeMusicManager::fetchApiKey()
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

void YouTubeMusicManager::searchForArtists(const QString &searchQuery)
{
    QUrl url(QStringLiteral(YTMUSIC_URL YTMUSIC_API_SEARCH));
    QUrlQuery query;
    query.addQueryItem("alt", "json");
    query.addQueryItem("key", m_apiKey);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral(CONTENTTYPE_JSON));
    request.setRawHeader("Referer", QByteArrayLiteral(YTMUSIC_URL));

    QNetworkReply *reply = m_netManager->post(request, SearchRequest().toData(searchQuery));

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        const QByteArray response = reply->readAll();

        QFile file("response.json");
        file.open(QIODevice::ReadWrite);
        file.write(response);
        file.close();

        qDebug() << response;
        auto doc = QJsonDocument::fromJson(response);
        auto result = YTMSearchResult::fromJson(doc.object());
        qDebug() << result.contents().length();
        for (const auto &shelf : result.contents()) {
            qDebug() << "[[" << shelf.title() << "]]";

            for (const auto &item : shelf.contents())
                qDebug() << item.title();
        }

        reply->deleteLater();
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError) {
        reply->deleteLater();
    });
}

/*
{
	"context": {
		"client": {
			"clientName": "WEB_REMIX",
			"clientVersion": "0.1",
			"hl": "en",
			"gl": "DE",
			"experimentIds": [],
			"experimentsToken": "",
			"utcOffsetMinutes": 60,
			"locationInfo": {
				"locationPermissionAuthorizationStatus": "LOCATION_PERMISSION_AUTHORIZATION_STATUS_UNSUPPORTED"
			},
			"musicAppInfo": {
				"musicActivityMasterSwitch": "MUSIC_ACTIVITY_MASTER_SWITCH_INDETERMINATE",
				"musicLocationMasterSwitch": "MUSIC_LOCATION_MASTER_SWITCH_INDETERMINATE",
				"pwaInstallabilityStatus": "PWA_INSTALLABILITY_STATUS_UNKNOWN"
			}
		},
		"capabilities": {},
		"request": {
			"internalExperimentFlags": [
				{"key": "force_music_enable_outertube_tastebuilder_browse", "value": "true"},
				{"key": "force_music_enable_outertube_playlist_detail_browse", "value": "true"},
				{"key": "force_music_enable_outertube_search_suggestions", "value": "true"}
			],
			"sessionIndex": 0
		},
		"clickTracking": {
			"clickTrackingParams": "CAQQ_V0YAyITCNuO97PAy-UCFfSs3godU_0MAg=="
		},
		"user": {
			"enableSafetyMode": false
		}
	},
	"query": "lindemann",
	"params": "Eg-KAQwIABAAGAAgASgAMABqChAEEAMQCRAFEAo%3D"
}
*/

void YouTubeMusicManager::musicInfo()
{
    QUrl url("https://www.youtube.com/get_video_info");
    QUrlQuery query;
    query.addQueryItem("html5", QString::number(1));
    query.addQueryItem("video_id", "4r9bF2FyoDc");
    query.addQueryItem("cpn", "NNLurNvlpUC0BuRS");
    query.addQueryItem("eurl", {});
    query.addQueryItem("el", "detailpage");
    query.addQueryItem("hl", "en_US");
    query.addQueryItem("sts", "18200");
    query.addQueryItem("lact", "1108");
    query.addQueryItem("c", "WEB_REMIX");
    query.addQueryItem("cver", "0.1");
    query.addQueryItem("cplayer", "UNIPLAYER");
    query.addQueryItem("player_params", "igMDCNgE");
    url.setQuery(query);

    QNetworkRequest request(url);

    QNetworkReply *reply = m_netManager->get(request);

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        const QByteArray response = reply->readAll();

        QFile file("response.x-www-formdata");
        file.open(QIODevice::ReadWrite);
        file.write(response);
        file.close();

        const QUrlQuery query(response);
        for (const auto &pair : query.queryItems()) {
            qDebug() << pair.first << pair.second;
            if (pair.first == "player_response") {
                qDebug() << "";

                QFile file("response.json");
                file.open(QIODevice::ReadWrite);
                file.write(QUrl::fromPercentEncoding(pair.second.toUtf8()).toUtf8());
                file.close();

//                 const QUrlQuery fflagsQuery(pair.second);
//                 for (const auto &pair : fflagsQuery.queryItems()) {
//                     qDebug() << pair.first << pair.second;
//                 }
                qDebug() << "";
            }
        }

        QJsonDocument doc = QJsonDocument::fromJson(response);
//         qDebug() << response;

        reply->deleteLater();
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError) {
        reply->deleteLater();
    });
}

