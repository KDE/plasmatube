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

#include "YTMSearchRequest.h"

#include <QLocale>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>
#include <QSharedData>

#include "constants.h"

class YTMSearchRequestPrivate : public QSharedData
{
public:
    QString query;
    QString params;
};

YTMSearchRequest::YTMSearchRequest(const QString &query, const QString &params)
    : YTMApiRequest(),
      d(new YTMSearchRequestPrivate)
{
    d->query = query;
    d->params = params;
}

YTMSearchRequest::YTMSearchRequest(const YTMSearchRequest &other) = default;

YTMSearchRequest::~YTMSearchRequest() = default;

YTMSearchRequest &YTMSearchRequest::operator=(const YTMSearchRequest &other) = default;

YTMSearchRequest YTMSearchRequest::fromJson(const QJsonObject &json)
{
    return YTMSearchRequest(
        json.value(QStringLiteral("query")).toString(),
        json.value(QStringLiteral("params")).toString()
    );
}

QJsonObject YTMSearchRequest::toJson() const
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

    QJsonObject json;
    json.insert("context", contextObj);
    json.insert("query", d->query);
    json.insert("params", d->params);

    return json;
}

QString YTMSearchRequest::urlPath(const QString& apiKey) const
{
    QUrlQuery query;
    query.addQueryItem("alt", "json");
    query.addQueryItem("key", apiKey);

    return QStringLiteral(YTMUSIC_API_SEARCH "?").append(query.toString(QUrl::FullyEncoded));
}

bool YTMSearchRequest::isNull() const
{
    return d->query.isEmpty() && d->params.isEmpty();
}

CREATE_GETTER_AND_SETTER(YTMSearchRequest, QString, d->query, query, setQuery)
CREATE_GETTER_AND_SETTER(YTMSearchRequest, QString, d->params, params, setParams)
