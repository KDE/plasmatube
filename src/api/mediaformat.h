// SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qinvidiousglobal.h"

#include <QDateTime>
#include <QUrl>

#include <cstdint>

namespace QInvidious
{

class MediaFormatBase
{
public:
    enum Container : uint8_t {
        InvalidContainer,
        WebM,
        Mp4,
        M4a,
    };

    enum Codec : uint8_t {
        InvalidCodec,
        Opus,
        AAC,
        VP9,
        H264,
        AV1,
    };

    FROM_JSON_OVERLOADS(MediaFormatBase)
    static MediaFormatBase fromJson(const QJsonObject &obj, MediaFormatBase &);

    QUrl url() const;

    uint16_t formatId() const;

    uint8_t fps() const;

    uint16_t resolution() const;

    Container container() const;

private:
    QUrl m_url;
    uint16_t m_formatId = 0;
    uint8_t m_fps = 0;
    uint16_t m_resolution = 0;
    Container m_container = InvalidContainer;
};

class MediaFormat : public MediaFormatBase
{
public:
    FROM_JSON_OVERLOADS(MediaFormat)
    static MediaFormat fromJson(const QJsonObject &obj, MediaFormat &);

    enum MediaType {
        Unknown,
        Audio,
        Video,
    };

    MediaType mediaType() const;

    uint32_t bitrate() const;
    void setBitrate(uint32_t bitrate);

    uint64_t size() const;
    void setSize(uint64_t size);

    Codec codec() const;
    void setCodec(Codec codec);

    QDateTime createdTimestamp() const;
    void setCreatedTimestamp(const QDateTime &createdTimestamp);

private:
    uint32_t m_bitrate = 0;
    uint64_t m_size = 0;
    Codec m_codec = InvalidCodec;
    QDateTime m_createdTimestamp;
};

class MediaFormatCombined : public MediaFormatBase
{
public:
    FROM_JSON_OVERLOADS(MediaFormatCombined)
    static MediaFormatCombined fromJson(const QJsonObject &obj, MediaFormatCombined &);

    Codec audioCodec() const;
    void setAudioCodec(Codec audioCodec);

    Codec videoCodec() const;
    void setVideoCodec(Codec videoCodec);

private:
    Codec m_audioCodec = InvalidCodec;
    Codec m_videoCodec = InvalidCodec;
};

}
