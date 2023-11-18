// SPDX-FileCopyrightText: none
// SPDX-License-Identifier: GPL-3.0-or-later

#include "searchparameters.h"

void SearchParameters::fill(const SearchParameters &other)
{
    m_query = other.m_query;
    m_page = other.m_page;
    m_sortBy = other.m_sortBy;
    m_date = other.m_date;
    m_duration = other.m_duration;
    m_type = other.m_type;
    m_features = other.m_features;
}

void SearchParameters::clear()
{
    m_query = QStringLiteral("");
    m_page = 0;
    m_sortBy = SortBy::Default;
    m_date = Date::Default;
    m_duration = Duration::Default;
    m_type = Type::Default;
    m_features.clear();
}

QHash<QString, QString> SearchParameters::toQueryParameters() const
{
    QHash<QString, QString> parameters;
    parameters.insert(QStringLiteral("q"), QString::fromUtf8(QUrl::toPercentEncoding(m_query)));
    parameters.insert(QStringLiteral("page"), QString::number(m_page));

    switch (m_sortBy) {
    case SortBy::Default:
        break;
    case SortBy::Relevance:
        parameters.insert(QStringLiteral("sort_by"), QStringLiteral("relevance"));
        break;
    case SortBy::Rating:
        parameters.insert(QStringLiteral("sort_by"), QStringLiteral("rating"));
        break;
    case SortBy::UploadDate:
        parameters.insert(QStringLiteral("sort_by"), QStringLiteral("upload_date"));
        break;
    case SortBy::ViewCount:
        parameters.insert(QStringLiteral("sort_by"), QStringLiteral("view_count"));
        break;
    }

    switch (m_date) {
    case Date::Default:
        break;
    case Date::Hour:
        parameters.insert(QStringLiteral("date"), QStringLiteral("hour"));
        break;
    case Date::Today:
        parameters.insert(QStringLiteral("date"), QStringLiteral("today"));
        break;
    case Date::Week:
        parameters.insert(QStringLiteral("date"), QStringLiteral("week"));
        break;
    case Date::Month:
        parameters.insert(QStringLiteral("date"), QStringLiteral("month"));
        break;
    case Date::Year:
        parameters.insert(QStringLiteral("date"), QStringLiteral("year"));
        break;
    }

    switch (m_duration) {
    case Duration::Default:
        break;
    case Duration::Short:
        parameters.insert(QStringLiteral("duration"), QStringLiteral("short"));
        break;
    case Duration::Long:
        parameters.insert(QStringLiteral("duration"), QStringLiteral("long"));
        break;
    }

    switch (m_type) {
    case Type::Default:
        break;
    case Type::Video:
        parameters.insert(QStringLiteral("type"), QStringLiteral("video"));
        break;
    case Type::Playlist:
        parameters.insert(QStringLiteral("type"), QStringLiteral("playlist"));
        break;
    case Type::Channel:
        parameters.insert(QStringLiteral("type"), QStringLiteral("channel"));
        break;
    case Type::All:
        parameters.insert(QStringLiteral("type"), QStringLiteral("all"));
        break;
    }

    if (!m_features.empty()) {
        QString featureList = QStringLiteral("");
        for (auto feature : m_features) {
            if (!featureList.isEmpty()) {
                featureList.append(QStringLiteral(","));
            }
            switch (feature) {
            case Feature::Hd:
                featureList.append(QStringLiteral("hd"));
                break;
            case Feature::Subtitles:
                featureList.append(QStringLiteral("subtitles"));
                break;
            case Feature::CreativeCommons:
                featureList.append(QStringLiteral("creative_commons"));
                break;
            case Feature::ThreeD:
                featureList.append(QStringLiteral("3d"));
                break;
            case Feature::Live:
                featureList.append(QStringLiteral("live"));
                break;
            case Feature::Purchased:
                featureList.append(QStringLiteral("purchased"));
                break;
            case Feature::FourK:
                featureList.append(QStringLiteral("4k"));
                break;
            case Feature::ThreeSixty:
                featureList.append(QStringLiteral("360"));
                break;
            case Feature::Location:
                featureList.append(QStringLiteral("location"));
                break;
            case Feature::Hdr:
                featureList.append(QStringLiteral("hdr"));
                break;
            }
        }
        parameters.insert(QStringLiteral("features"), featureList);
    }
    return parameters;
}

QHash<QString, QString> SearchParameters::toPipedQueryParameters() const
{
    QHash<QString, QString> parameters;
    parameters.insert(QStringLiteral("q"), QString::fromUtf8(QUrl::toPercentEncoding(m_query)));
    parameters.insert(QStringLiteral("filter"), QString::fromUtf8(QUrl::toPercentEncoding(m_query)));
    return parameters;
}

const QString &SearchParameters::query() const
{
    return m_query;
}
void SearchParameters::setQuery(const QString &query)
{
    m_query = query;
}

unsigned int SearchParameters::page() const
{
    return m_page;
}
void SearchParameters::setPage(unsigned int page)
{
    m_page = page;
}

SearchParameters::SortBy SearchParameters::sortBy() const
{
    return m_sortBy;
}
void SearchParameters::setSortBy(SearchParameters::SortBy sortBy)
{
    m_sortBy = sortBy;
}

SearchParameters::Date SearchParameters::date() const
{
    return m_date;
}
void SearchParameters::setDate(SearchParameters::Date date)
{
    m_date = date;
}

SearchParameters::Duration SearchParameters::duration() const
{
    return m_duration;
}
void SearchParameters::setDuration(SearchParameters::Duration duration)
{
    m_duration = duration;
}

SearchParameters::Type SearchParameters::type() const
{
    return m_type;
}
void SearchParameters::setType(SearchParameters::Type type)
{
    m_type = type;
}

const QVector<SearchParameters::Feature> &SearchParameters::features()
{
    return m_features;
}
void SearchParameters::setFeatures(const QVector<SearchParameters::Feature> &features)
{
    m_features = features;
}
