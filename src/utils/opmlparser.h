// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QList>
#include <QString>

#include <optional>

#include "api/channel.h"

namespace OPMLParser
{
/**
 * Parses a YouTube subscription OPML from @p filePath.
 * @return A variant containing either the list of channels to subscribe to, or an error.
 */
std::variant<QList<QString>, QString> parseOPMLSubscriptions(const QString &filePath);

/**
 * Saves the subscriptions from @p source to @p filePath.
 * @return An error, if any.
 */
std::optional<QString> saveOPMLSubscriptions(const QList<QInvidious::Channel> &channels, const QString &filePath);
}