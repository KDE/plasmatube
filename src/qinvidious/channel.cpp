// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "channel.h"

using namespace QInvidious;
using namespace Qt::StringLiterals;

Channel Channel::fromJson(const QJsonObject &obj, Channel &channel)
{
    channel.setName(obj["author"_L1].toString());
    const QJsonValue firstAvatar = obj["authorThumbnails"_L1].toArray().first();
    channel.setAvatar(firstAvatar.toObject()["url"_L1].toString());
    const QJsonValue firstBanners = obj["authorBanners"_L1].toArray().first();
    channel.setBanner(firstBanners.toObject()["url"_L1].toString());
    channel.setDescription(obj["description"_L1].toString());
    channel.setSubCount(obj["subCount"_L1].toInt());
    return channel;
}

QString Channel::name() const
{
    return m_name;
}

void Channel::setName(const QString &name)
{
    m_name = name;
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

void Channel::setDescription(const QString &description)
{
    m_description = description;
}

int Channel::subCount() const
{
    return m_subCount;
}

void Channel::setSubCount(int count)
{
    m_subCount = count;
}