// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

namespace QInvidious
{
class InvidiousApi;
class PeerTubeApi;
class PipedApi;
}

class Paginator : public QObject
{
public:
    explicit Paginator(QObject *parent);
    ~Paginator() override = default;

    /**
     * @brief Resets the paginator to it's first page or index.
     */
    void reset();

    /**
     * @brief Goes to the next page, or it's equivalent depending on the paginator type.
     */
    void next();

    /**
     * @brief Goes to the previous page, or it's equivalent depending on the paginator type.
     */
    void previous();

    /**
     * @return If there's any more pages to paginate through.
     */
    bool hasMore() const;

protected:
    enum class Type {
        Page, /**< Based on pages. Pages start at 0, and next() increases the page number. */
        StartIndex, /**< Based on indexes. The start index starts at 0, and next() increases the startIndex based on the previous result count. */
        Continuation /**< Based on a continuation URL. next() switches the url for the result. */
    };

    void setType(Type type);

    Type m_type = Type::Page;

    // Page
    qsizetype m_page = 0;
    qsizetype m_maxPage = 0;

    // Start Index
    qsizetype m_startIndex = 0;
    qsizetype m_count = 0;
    qsizetype m_total = 0;

    // Pagination
    QString m_continuation;

    friend class QInvidious::InvidiousApi;
    friend class QInvidious::PeerTubeApi;
    friend class QInvidious::PipedApi;
};