// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qinvidiousglobal.h"

#include <QUrl>

namespace QInvidious
{

class Caption
{
public:
    FROM_JSON_OVERLOADS(Caption)
    static Caption fromJson(const QJsonObject &obj, Caption &);

    QString label() const;

    QString languageCode() const;

    QUrl url() const;

private:
    QString m_label;
    QString m_languageCode;
    QUrl m_url;
};

}
