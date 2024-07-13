// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sponsorblockcontroller.h"

#include "plasmatube.h"

SponsorBlockController::SponsorBlockController(QObject *parent)
    : QObject(parent)
{
}

void SponsorBlockController::requestSponsors(const QString &videoId)
{
    m_sponsors = {};

    auto source = PlasmaTube::instance().selectedSource();
    if (!source->api()->supportsFeature(QInvidious::AbstractApi::SponsorBlock)) {
        return;
    }

    if (!PlasmaTube::instance().settings()->useSponsorBlock()) {
        return;
    }

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("videoID"), videoId);

    QUrl sponsorRequestUrl;
    sponsorRequestUrl.setScheme(QStringLiteral("https"));
    sponsorRequestUrl.setHost(PlasmaTube::instance().settings()->sponsorBlockServer());
    sponsorRequestUrl.setPath(QStringLiteral("/api/skipSegments"));
    sponsorRequestUrl.setQuery(query);

    auto reply = PlasmaTube::instance().networkAccessManager()->get(QNetworkRequest(sponsorRequestUrl));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (auto doc = QJsonDocument::fromJson(reply->readAll()); !doc.isNull()) {
            Sponsors sponsors;

            for (auto segment : doc.array()) {
                Segment seg{.start = segment.toObject().value(QLatin1String("segment")).toArray().first().toDouble(),
                            .end = segment.toObject().value(QLatin1String("segment")).toArray().last().toDouble()};
                sponsors.segments.push_back(seg);
            }

            m_sponsors = sponsors;
        }
    });
}

bool SponsorBlockController::isSponsor(const qreal position) const
{
    if (!m_sponsors.has_value()) {
        return {};
    }

    return findSegment(position).has_value();
}

std::optional<qreal> SponsorBlockController::findFreePosition(const qreal position) const
{
    if (!m_sponsors.has_value()) {
        return {};
    }

    auto segment = findSegment(position);
    if (!segment) {
        return {};
    }

    return segment->end;
}

std::optional<SponsorBlockController::Segment> SponsorBlockController::findSegment(const qreal position) const
{
    if (!m_sponsors.has_value()) {
        return {};
    }

    for (const auto &segment : m_sponsors->segments) {
        if (position >= segment.start && position < segment.end) {
            return segment;
        }
    }

    return {};
}

#include "moc_sponsorblockcontroller.cpp"