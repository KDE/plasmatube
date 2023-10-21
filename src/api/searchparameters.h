// SPDX-FileCopyrightText: none
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

#include <QHash>
#include <QString>
#include <QUrl>
#include <QVector>

class SearchParameters : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString query READ query WRITE setQuery)
    Q_PROPERTY(unsigned int page READ page WRITE setPage)
    Q_PROPERTY(SortBy sortBy READ sortBy WRITE setSortBy)
    Q_PROPERTY(Date date READ date WRITE setDate)
    Q_PROPERTY(Duration duration READ duration WRITE setDuration)
    Q_PROPERTY(Type type READ type WRITE setType)
    Q_PROPERTY(QVector<Feature> features READ features WRITE setFeatures)

public:
    enum class SortBy { Default, Relevance, Rating, UploadDate, ViewCount };
    Q_ENUM(SortBy)

    enum class Date { Default, Hour, Today, Week, Month, Year };
    Q_ENUM(Date)

    enum class Duration { Default, Short, Long };
    Q_ENUM(Duration)

    enum class Type { Default, Video, Playlist, Channel, All };
    Q_ENUM(Type)

    enum class Feature { Hd, Subtitles, CreativeCommons, ThreeD, Live, Purchased, FourK, ThreeSixty, Location, Hdr };
    Q_ENUM(Feature)

    SearchParameters()
    {
    }

    explicit SearchParameters(const QString &query, unsigned int page = 0)
        : m_query(query)
        , m_page(page)
    {
    }

    void fill(const SearchParameters &other); // cannot use copy constructor with Q_OBJECT
    void clear();

    QHash<QString, QString> toQueryParameters() const;

    const QString &query() const;
    void setQuery(const QString &query);

    unsigned int page() const;
    void setPage(unsigned int page);

    SortBy sortBy() const;
    void setSortBy(SortBy sortBy);

    Date date() const;
    void setDate(Date date);

    Duration duration() const;
    void setDuration(Duration duration);

    Type type() const;
    void setType(Type type);

    const QVector<Feature> &features();
    void setFeatures(const QVector<Feature> &features);

private:
    QString m_query;
    unsigned int m_page = 1;
    SortBy m_sortBy = SortBy::Default;
    Date m_date = Date::Default;
    Duration m_duration = Duration::Default;
    Type m_type = Type::Default;
    QVector<Feature> m_features;
};
