// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "paginator.h"

Paginator::Paginator(QObject *parent)
    : QObject(parent)
{
}

void Paginator::reset()
{
    m_page = 0;
    m_startIndex = 0;
    m_continuation.clear();
}

void Paginator::next()
{
    switch (m_type) {
    case Type::Page:
        m_page++;
        break;
    case Type::StartIndex:
        m_startIndex += m_count;
        break;
    case Type::Continuation:
        break;
    }
}

void Paginator::previous()
{
}

bool Paginator::hasMore() const
{
    switch (m_type) {
    case Type::Page:
        return m_page < m_maxPage;
    case Type::StartIndex:
        return m_startIndex + m_count < m_total;
    case Type::Continuation:
        break;
    }

    return true;
}

void Paginator::setType(Paginator::Type type)
{
    m_type = type;
}
