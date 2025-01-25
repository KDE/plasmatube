// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "channel.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

Channel Channel::fromJson(const QJsonObject &obj, Channel &channel)
{
    const bool isPeerTube = obj.contains("avatars"_L1);
    const bool isPiped = obj.contains("url"_L1) || obj.contains("nextpage"_L1);
    if (isPeerTube) {
        channel.m_id = obj["name"_L1].toString() + u"@" + obj["host"_L1].toString();
        channel.m_name = obj["displayName"_L1].toString();
        channel.m_description = obj["description"_L1].toString();
        channel.m_subCount = obj["followersCount"_L1].toInt();
        channel.m_avatar = obj["avatars"_L1].toArray()[0].toObject()["path"_L1].toString();
        const QJsonValue firstBanners = obj["banners"_L1].toArray().first();
        channel.m_banner = firstBanners.toObject()["path"_L1].toString();
    } else if (isPiped) {
        channel.m_id = obj["url"_L1].toString().remove(QStringLiteral("/channel/"));
        channel.m_name = obj["name"_L1].toString();
        channel.m_avatar = obj["avatarUrl"_L1].toString();
        channel.m_banner = obj["bannerUrl"_L1].toString();
        channel.m_description = obj["description"_L1].toString();
        channel.m_subCount = obj["subscriberCount"_L1].toInt();
    } else {
        channel.m_id = obj["authorId"_L1].toString();
        channel.m_name = obj["author"_L1].toString();
        const QJsonValue firstAvatar = obj["authorThumbnails"_L1].toArray().first();
        channel.m_avatar = firstAvatar.toObject()["url"_L1].toString();
        const QJsonValue firstBanners = obj["authorBanners"_L1].toArray().first();
        channel.m_banner = firstBanners.toObject()["url"_L1].toString();
        channel.m_description = obj["description"_L1].toString();
        channel.m_subCount = obj["subCount"_L1].toInt();
    }

    return channel;
}

QString Channel::id() const
{
    return m_id;
}

QString Channel::name() const
{
    return m_name;
}

QString Channel::banner() const
{
    return m_banner;
}

void Channel::setBanner(const QString &banner)
{
    m_banner = banner;
}

QString Channel::avatar() const
{
    return m_avatar;
}

void Channel::setAvatar(const QString &avatar)
{
    m_avatar = avatar;
}

QString Channel::description() const
{
    return m_description;
}

int Channel::subCount() const
{
    return m_subCount;
}
