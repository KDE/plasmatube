// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <MpvAbstractItem>

#include <QQuickFramebufferObject>

class MpvObject : public MpvAbstractItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(qreal position READ position NOTIFY positionChanged)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged)
    Q_PROPERTY(bool stopped READ stopped NOTIFY stoppedChanged)

public:
    explicit MpvObject(QQuickItem *parent = nullptr);
    ~MpvObject() override = default;

    void setAudioUrl(const QString &audioUrl);

    qreal position() const;
    qreal duration() const;
    bool paused() const;
    bool stopped() const;

public Q_SLOTS:
    void play();
    void pause();
    void stop();
    void setPosition(double value);
    void seek(qreal offset);
    void toggleStats();

Q_SIGNALS:
    void positionChanged();
    void durationChanged();
    void pausedChanged();
    void stoppedChanged();

private:
    void onPropertyChanged(const QString &property, const QVariant &value);

    bool m_paused = true;
    qreal m_position = 0;
    qreal m_duration = 0;
    bool m_stopped = true;
    QString m_audioUrl;
};