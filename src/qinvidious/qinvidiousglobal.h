// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#define FROM_JSON_OVERLOADS(className) \
    static className fromJson(const QByteArray &bytes) \
    { \
        return fromJson(QJsonDocument::fromJson(bytes)); \
    } \
    static className fromJson(const QJsonDocument &doc) \
    { \
        return fromJson(doc.object()); \
    } \
    static className fromJson(const QJsonObject &obj) \
    { \
        className item; \
        return fromJson(obj, item); \
    } \
    static className fromJson(const QJsonValue &val) \
    { \
        return fromJson(val.toObject()); \
    } \
    static QList<className> fromJson(const QJsonArray &array) \
    { \
        QList<className> items; \
        for (const auto &val : array) { \
            items << fromJson(val); \
        } \
        return items; \
    }
