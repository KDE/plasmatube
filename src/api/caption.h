// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QUrl>

#include "qinvidiousglobal.h"

namespace QInvidious
{

class Caption
{
public:
    FROM_JSON_OVERLOADS(Caption)
    static Caption fromJson(const QJsonObject &obj, Caption &);

    Caption();

    QString label() const;
    void setLabel(const QString &label);

    QString languageCode() const;
    void setLanguageCode(const QString &languageCode);

    QUrl url() const;
    void setUrl(const QUrl &url);

private:
    QString m_label;
    QString m_languageCode;
    QUrl m_url;
};

}
