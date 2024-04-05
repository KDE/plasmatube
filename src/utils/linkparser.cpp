// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils/linkparser.h"

#include <QRegularExpression>

namespace LinkParser
{

std::optional<QString> parseVideoString(const QString &vid)
{
    const QRegularExpression exp(QStringLiteral(R"(https:\/\/[www.]*youtube.com\/watch\?v=(.*))"));
    const auto match = exp.match(vid);
    if (match.hasMatch()) {
        return match.captured(1);
    }

    return std::nullopt;
}

}