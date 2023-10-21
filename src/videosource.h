// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDir>
#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "sourceconfig.h"

namespace QInvidious
{
class AbstractApi;
}

class VideoSource : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Use from SourceManager")

    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)

public:
    explicit VideoSource(const QString &key, QObject *parent = nullptr);

    enum class Type { Invidious, PeerTube };
    Q_ENUM(Type)

    [[nodiscard]] QString uuid() const;

    [[nodiscard]] QString url() const;
    void setUrl(const QString &url);

    [[nodiscard]] Type type() const;
    void setType(Type type);

    QInvidious::AbstractApi *api() const;

Q_SIGNALS:
    void urlChanged();
    void typeChanged();
    void credentialsChanged();

private:
    void createApi();

    SourceConfig m_config;
    QString m_key;
    QInvidious::AbstractApi *m_api;
};
