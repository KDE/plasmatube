// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

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

namespace QInvidious
{

template <typename T>
void parseArray(const QJsonValue &value, QList<T> &values)
{
    return parseArray<T>(value.toArray(), values);
}

template <typename T>
void parseArray(const QJsonArray &array, QList<T> &values)
{
    std::transform(array.cbegin(), array.cend(), std::back_inserter(values),
                   [](const QJsonValue &val) {
        return T::fromJson(val);
    });
}

inline void parseArray(const QJsonArray &array, QList<QString> &values)
{
    std::transform(array.cbegin(), array.cend(), std::back_inserter(values),
                   [](const QJsonValue &val) {
        return val.toString();
    });
}

}
