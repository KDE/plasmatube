// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class SponsorBlockController : public QObject
{
    Q_OBJECT

public:
    explicit SponsorBlockController(QObject *parent = nullptr);

    void requestSponsors(const QString &videoId);

    bool isSponsor(qreal position) const;
    std::optional<qreal> findFreePosition(qreal position) const;

private:
    struct Segment {
        qreal start, end;
    };

    struct Sponsors {
        QList<Segment> segments;
    };

    std::optional<Segment> findSegment(qreal position) const;

    std::optional<Sponsors> m_sponsors;
};
