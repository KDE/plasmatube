// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "caption.h"

using namespace QInvidious;

Caption Caption::fromJson(const QJsonObject &obj, Caption &caption)
{
    caption.m_label = obj.value(u"label").toString();
    caption.m_languageCode = obj.value(u"languageCode").toString();
    caption.m_url = QUrl(obj.value(u"url").toString());
    return caption;
}

QString Caption::label() const
{
    return m_label;
}

QString Caption::languageCode() const
{
    return m_languageCode;
}

QUrl Caption::url() const
{
    return m_url;
}
