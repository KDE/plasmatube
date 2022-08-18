// SPDX-FileCopyrightText: none
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "searchparameters.h"


void SearchParameters::fill(const SearchParameters &other) {
    m_query = other.m_query;
    m_page = other.m_page;
    m_sortBy = other.m_sortBy;
    m_date = other.m_date;
    m_duration = other.m_duration;
    m_type = other.m_type;
    m_features = other.m_features;
}

void SearchParameters::clear() {
    m_query = "";
    m_page = 0;
    m_sortBy = SortBy::Default;
    m_date = Date::Default;
    m_duration = Duration::Default;
    m_type = Type::Default;
    m_features.clear();
}

QHash<QString, QString> SearchParameters::toQueryParameters() const {
    QHash<QString, QString> parameters;
    parameters.insert("q", QUrl::toPercentEncoding(m_query));
    parameters.insert("page", QString::number(m_page));

    switch (m_sortBy) {
    case SortBy::Default:
        break;
    case SortBy::Relevance:
        parameters.insert("sort_by", "relevance");
        break;
    case SortBy::Rating:
        parameters.insert("sort_by", "rating");
        break;
    case SortBy::UploadDate:
        parameters.insert("sort_by", "upload_date");
        break;
    case SortBy::ViewCount:
        parameters.insert("sort_by", "view_count");
        break;
    }

    switch (m_date) {
    case Date::Default:
        break;
    case Date::Hour:
        parameters.insert("date", "hour");
        break;
    case Date::Today:
        parameters.insert("date", "today");
        break;
    case Date::Week:
        parameters.insert("date", "week");
        break;
    case Date::Month:
        parameters.insert("date", "month");
        break;
    case Date::Year:
        parameters.insert("date", "year");
        break;
    }

    switch (m_duration) {
    case Duration::Default:
        break;
    case Duration::Short:
        parameters.insert("duration", "short");
        break;
    case Duration::Long:
        parameters.insert("duration", "long");
        break;
    }

    switch (m_type) {
    case Type::Default:
        break;
    case Type::Video:
        parameters.insert("type", "video");
        break;
    case Type::Playlist:
        parameters.insert("type", "playlist");
        break;
    case Type::Channel:
        parameters.insert("type", "channel");
        break;
    case Type::All:
        parameters.insert("type", "all");
        break;
    }

    if (!m_features.empty()) {
        QString featureList = "";
        foreach (auto feature, m_features) {
            if (!featureList.isEmpty()) {
                featureList.append(",");
            }
            switch (feature) {
            case Feature::Hd:
                featureList.append("hd");
                break;
            case Feature::Subtitles:
                 featureList.append("subtitles");
                break;
            case Feature::CreativeCommons:
                 featureList.append("creative_commons");
                break;
            case Feature::ThreeD:
                 featureList.append("3d");
                break;
            case Feature::Live:
                 featureList.append("live");
                break;
            case Feature::Purchased:
                 featureList.append("purchased");
                break;
            case Feature::FourK:
                 featureList.append("4k");
                break;
            case Feature::ThreeSixty:
                 featureList.append("360");
                break;
            case Feature::Location:
                 featureList.append("location");
                break;
            case Feature::Hdr:
                 featureList.append("hdr");
                break;
            }
        }
        parameters.insert("features", featureList);
    }
    return parameters;
}

const QString& SearchParameters::query() const {
    return m_query;
}
void SearchParameters::setQuery(const QString& query) {
    m_query = query;
}

unsigned int SearchParameters::page() const {
    return m_page;
}
void SearchParameters::setPage(unsigned int page) {
    m_page = page;
}

SearchParameters::SortBy SearchParameters::sortBy() const {
    return m_sortBy;
}
void SearchParameters::setSortBy(SearchParameters::SortBy sortBy) {
    m_sortBy = sortBy;
}

SearchParameters::Date SearchParameters::date() const {
    return m_date;
}
void SearchParameters::setDate(SearchParameters::Date date) {
    m_date = date;
}

SearchParameters::Duration SearchParameters::duration() const {
    return m_duration;
}
void SearchParameters::setDuration(SearchParameters::Duration duration) {
    m_duration = duration;
}

SearchParameters::Type SearchParameters::type() const {
    return m_type;
}
void SearchParameters::setType(SearchParameters::Type type) {
    m_type = type;
}

const QVector<SearchParameters::Feature>& SearchParameters::features() {
    return m_features;
}
void SearchParameters::setFeatures(const QVector<SearchParameters::Feature> &features) {
    m_features = features;
}

