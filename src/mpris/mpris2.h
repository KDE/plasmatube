/**
 * SPDX-FileCopyrightText: 2014 Sujith Haridasan <sujith.haridasan@kdemail.net>
 * SPDX-FileCopyrightText: 2014 Ashish Madeti <ashishmadeti@gmail.com>
 * SPDX-FileCopyrightText: 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * SPDX-FileCopyrightText: 2022-2023 Bart De Vries <bart@mogwai.be>
 * SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <memory>

#include <QObject>
#include <QSharedPointer>

#if !defined Q_OS_ANDROID
class MediaPlayer2Player;
class MediaPlayer2;
#endif
class VideoController;

class Mpris2 : public QObject
{
    Q_OBJECT

public:
    explicit Mpris2(QObject *parent = nullptr);
    ~Mpris2() override;

private:
    void initDBusService();
    bool unregisterDBusService();

#if !defined Q_OS_ANDROID
    std::unique_ptr<MediaPlayer2> m_mp2;
    std::unique_ptr<MediaPlayer2Player> m_mp2p;
#endif

    VideoController *m_audioPlayer = nullptr;
    QString m_playerName;
};
