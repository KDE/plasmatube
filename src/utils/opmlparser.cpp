// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils/opmlparser.h"

#include <QFile>
#include <QUrlQuery>
#include <QXmlStreamReader>

std::variant<QList<QString>, QString> OPMLParser::parseOPMLSubscriptions(const QString &filePath)
{
    // TODO: support URLs?
    QString normalizedPath = filePath;
    if (normalizedPath.contains(QStringLiteral("file://"))) {
        normalizedPath.remove(QStringLiteral("file://"));
    }

    QFile file(normalizedPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return file.errorString();
    }

    QXmlStreamReader xmlReader(&file);

    QStringList ids;
    while (!xmlReader.atEnd()) {
        xmlReader.readNext();
        if (xmlReader.tokenType() == 4 && xmlReader.attributes().hasAttribute(QStringLiteral("xmlUrl"))) {
            QUrl url = QUrl::fromUserInput(xmlReader.attributes().value(QStringLiteral("xmlUrl")).toString());
            QUrlQuery query = QUrlQuery(url.query());
            ids << query.queryItemValue(QStringLiteral("channel_id"));
        }
    }

    if (xmlReader.hasError()) {
        return xmlReader.errorString();
    }

    return ids;
}

std::optional<QString> OPMLParser::saveOPMLSubscriptions(const QList<QInvidious::Channel> &channels, const QString &filePath)
{
    QString normalizedPath = filePath;
    if (normalizedPath.contains(QStringLiteral("file://"))) {
        normalizedPath.remove(QStringLiteral("file://"));
    }

    QFile file(normalizedPath);
    if (!file.open(QIODevice::WriteOnly)) {
        return file.errorString();
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument(QStringLiteral("1.0"));
    xmlWriter.writeStartElement(QStringLiteral("opml"));
    xmlWriter.writeAttribute(QStringLiteral("version"), QStringLiteral("1.1"));
    xmlWriter.writeStartElement(QStringLiteral("body"));
    xmlWriter.writeStartElement(QStringLiteral("outline"));
    xmlWriter.writeAttribute(QStringLiteral("title"), QStringLiteral("YouTube Subscriptions"));
    xmlWriter.writeAttribute(QStringLiteral("text"), QStringLiteral("YouTube Subscriptions"));
    for (const auto &channel : channels) {
        xmlWriter.writeEmptyElement(QStringLiteral("outline"));
        xmlWriter.writeAttribute(QStringLiteral("xmlUrl"), QStringLiteral("https://www.youtube.com/feeds/videos.xml?channel_id=%1").arg(channel.id()));
        xmlWriter.writeAttribute(QStringLiteral("title"), channel.name());
        xmlWriter.writeAttribute(QStringLiteral("text"), channel.name());
    }
    xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    // TODO: check for errors

    return std::nullopt;
}