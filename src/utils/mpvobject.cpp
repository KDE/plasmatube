// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mpvobject.h"

#include <MpvController>
#include <QDir>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QStandardPaths>

MpvObject::MpvObject(QQuickItem *parent)
    : MpvAbstractItem(parent)
{
    setProperty(QStringLiteral("terminal"), QStringLiteral("yes"));
    setProperty(QStringLiteral("keep-open"), QStringLiteral("always"));
    setProperty(QStringLiteral("audio-client-name"), QStringLiteral("org.kde.plasmatube"));
    setProperty(QStringLiteral("wayland-app-id"), QStringLiteral("org.kde.plasmatube"));

    observeProperty(QStringLiteral("duration"), MPV_FORMAT_DOUBLE);
    observeProperty(QStringLiteral("time-pos"), MPV_FORMAT_DOUBLE);
    observeProperty(QStringLiteral("pause"), MPV_FORMAT_FLAG);

    connect(mpvController(), &MpvController::propertyChanged, this, &MpvObject::onPropertyChanged, Qt::QueuedConnection);
}

qreal MpvObject::position()
{
    return m_position;
}

qreal MpvObject::duration()
{
    return m_duration;
}

bool MpvObject::paused()
{
    return m_paused;
}

void MpvObject::play()
{
    if (!paused()) {
        return;
    }
    setProperty(QStringLiteral("pause"), false);
    Q_EMIT pausedChanged();
}

void MpvObject::pause()
{
    if (paused()) {
        return;
    }
    setProperty(QStringLiteral("pause"), true);
    Q_EMIT pausedChanged();
}

void MpvObject::stop()
{
    setPosition(0);
    setProperty(QStringLiteral("stop"), true);
    m_stopped = true;
    Q_EMIT stoppedChanged();
}

void MpvObject::setPosition(double value)
{
    if (value == position()) {
        return;
    }
    setProperty(QStringLiteral("time-pos"), value);
    Q_EMIT positionChanged();
}

void MpvObject::seek(qreal offset)
{
    Q_EMIT command(QStringList() << QStringLiteral("add") << QStringLiteral("time-pos") << QString::number(offset));
}

bool MpvObject::stopped()
{
    return m_stopped;
}

void MpvObject::onPropertyChanged(const QString &property, const QVariant &value)
{
    if (property == QStringLiteral("time-pos")) {
        double time = value.toDouble();
        m_position = time;
        Q_EMIT positionChanged();

        if (m_position > 0.0 && m_stopped) {
            m_stopped = false;
            Q_EMIT stoppedChanged();
        }
    } else if (property == QStringLiteral("duration")) {
        double time = value.toDouble();
        m_duration = time;
        Q_EMIT durationChanged();
    } else if (property == QStringLiteral("pause")) {
        m_paused = value.toBool();
        Q_EMIT pausedChanged();
    }
}

#include "moc_mpvobject.cpp"