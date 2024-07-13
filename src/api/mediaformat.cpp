// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mediaformat.h"

#include <QJsonObject>

using namespace QInvidious;

MediaFormatBase::Container containerFromString(const QString &container)
{
    if (container == u"webm") {
        return MediaFormatBase::WebM;
    }
    if (container == u"mp4") {
        return MediaFormatBase::Mp4;
    }
    if (container == u"m4a") {
        return MediaFormatBase::M4a;
    }
    return MediaFormatBase::InvalidContainer;
}

MediaFormatBase::Codec codecFromString(const QString &codec)
{
    if (codec == u"opus") {
        return MediaFormatBase::Opus;
    }
    if (codec == u"aac" || codec.startsWith(u"mp4a")) {
        return MediaFormatBase::AAC;
    }
    if (codec == u"vp9") {
        return MediaFormatBase::VP9;
    }
    if (codec == u"h264" || codec.startsWith(u"avc1")) {
        return MediaFormatBase::H264;
    }
    if (codec.startsWith(u"av01")) {
        return MediaFormatBase::AV1;
    }
    return MediaFormatBase::InvalidCodec;
}

MediaFormatBase MediaFormatBase::fromJson(const QJsonObject &obj, MediaFormatBase &format)
{
    format.m_url = QUrl(obj.value(u"url").toString());
    format.m_formatId = obj.value(u"itag").toInt();
    format.m_fps = obj.value(u"fps").toInt();
    // remove 'p' from "2160p", resolution doesn't contain something like "720p60"
    if (const auto resolution = obj.value(u"resolution").toString(); resolution.size() > 1) {
        format.m_resolution = resolution.chopped(1).toUInt();
    }
    format.m_container = containerFromString(obj.value(u"container").toString());
    format.m_qualityLabel = obj.value(u"qualityLabel").toString();
    return format;
}

QUrl MediaFormatBase::url() const
{
    return m_url;
}

void MediaFormatBase::setUrl(QUrl url)
{
    m_url = url;
}

uint16_t MediaFormatBase::formatId() const
{
    return m_formatId;
}

uint8_t MediaFormatBase::fps() const
{
    return m_fps;
}

uint16_t MediaFormatBase::resolution() const
{
    return m_resolution;
}

auto MediaFormatBase::container() const -> Container
{
    return m_container;
}

QString MediaFormatBase::qualityLabel() const
{
    return m_qualityLabel;
}

void MediaFormatBase::setQualityLabel(const QString &label)
{
    m_qualityLabel = label;
}

MediaFormat MediaFormat::fromJson(const QJsonObject &obj, MediaFormat &format)
{
    MediaFormatBase::fromJson(obj, format);
    format.m_bitrate = obj.value(u"bitrate").toInt();
    format.m_size = obj.value(u"clen").toInt();
    format.m_codec = codecFromString(obj.value(u"encoding").toString());
    // the value is provided as a string and in nanoseconds
    format.m_createdTimestamp = QDateTime::fromMSecsSinceEpoch(QString(obj.value(u"lmt").toString()).toULongLong() / 1000);
    return format;
}

MediaFormat::MediaType MediaFormat::mediaType() const
{
    switch (m_codec) {
    case Opus:
    case AAC:
        return Audio;
    case VP9:
    case H264:
    case AV1:
        return Video;
    case InvalidCodec:
        break;
    }
    return Unknown;
}

uint32_t MediaFormat::bitrate() const
{
    return m_bitrate;
}

void MediaFormat::setBitrate(uint32_t bitrate)
{
    m_bitrate = bitrate;
}

uint64_t MediaFormat::size() const
{
    return m_size;
}

void MediaFormat::setSize(uint64_t size)
{
    m_size = size;
}

auto MediaFormat::codec() const -> Codec
{
    return m_codec;
}

void MediaFormat::setCodec(Codec codec)
{
    m_codec = codec;
}

QDateTime MediaFormat::createdTimestamp() const
{
    return m_createdTimestamp;
}

void MediaFormat::setCreatedTimestamp(const QDateTime &createdTimestamp)
{
    m_createdTimestamp = createdTimestamp;
}

MediaFormatCombined MediaFormatCombined::fromJson(const QJsonObject &obj, MediaFormatCombined &format)
{
    MediaFormatBase::fromJson(obj, format);
    // something like: "video/mp4; codecs=\"avc1.42001E, mp4a.40.2\""
    const auto codecsStr = obj.value(u"format").toString();
    const QString &codecs = codecsStr;
    const auto codecsStart = codecs.indexOf(u'"');
    const auto codecsLength = codecsStart - codecs.indexOf(u'"', codecsStart);
    const auto codecList = codecs.mid(codecsStart, codecsLength).split(QLatin1String(", "));
    if (!codecList.empty()) {
        format.m_videoCodec = codecFromString(codecList[0]);
        if (codecList.size() >= 2) {
            format.m_audioCodec = codecFromString(codecList[1]);
        }
    }
    return format;
}

auto MediaFormatCombined::audioCodec() const -> Codec
{
    return m_audioCodec;
}

void MediaFormatCombined::setAudioCodec(Codec audioCodec)
{
    m_audioCodec = audioCodec;
}

auto MediaFormatCombined::videoCodec() const -> Codec
{
    return m_videoCodec;
}

void MediaFormatCombined::setVideoCodec(Codec videoCodec)
{
    m_videoCodec = videoCodec;
}
