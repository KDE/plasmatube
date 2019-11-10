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


#ifndef YTMSEARCHREQUEST_H
#define YTMSEARCHREQUEST_H

#include <QString>

#include "YTMApiRequest.h"

class YTMSearchRequest : public YTMApiRequest
{
public:
    YTMSearchRequest(const QString &query = {}, const QString &params = {});
    ~YTMSearchRequest() override;

    QJsonObject toJson() const override;

    QString urlPath(const QString &apiKey) const override;

    QString query() const;
    void setQuery(const QString &query);

    QString params() const;
    void setParams(const QString &params);

private:
    QString m_query;
    QString m_params;
};

#endif // YTMSEARCHREQUEST_H
