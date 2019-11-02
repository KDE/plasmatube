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

void YouTubeMusicManager::searchForArtists(const QString &searchQuery)
{
    QUrl url("https://music.youtube.com/youtubei/v1/search");
    QUrlQuery query;
    query.addQueryItem("alt", "json");
//     query.addQueryItem("key", "AIzaSyC9XL3ZjWddXya6X74dJoCTL-WEYFDNX30");
    query.addQueryItem("key", "");
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Referer", "https://music.youtube.com");


    QNetworkReply *reply = m_netManager->post(request, SearchRequest().toData(searchQuery));

    // success
    connect(reply, &QNetworkReply::finished, this, [=] () {
        const QByteArray response = reply->readAll();

        QFile file("response.json");
        file.open(QIODevice::ReadWrite);
        file.write(response);
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(response);
        qDebug() << response;

        reply->deleteLater();
    });

    // failure
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, [=] (QNetworkReply::NetworkError) {
        reply->deleteLater();
    });

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
}

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

