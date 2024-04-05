// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "utils/linkparser.h"

class LinkParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testYoutubeLink()
    {
        QCOMPARE_EQ(LinkParser::parseVideoString(QStringLiteral("https://www.youtube.com/watch?v=aqz-KE-bpKQ")), QStringLiteral("aqz-KE-bpKQ"));
    }
};

QTEST_MAIN(LinkParserTest)
#include "linkparsertest.moc"
