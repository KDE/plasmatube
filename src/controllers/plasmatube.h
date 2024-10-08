// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "config.h"
#include "sourcemanager.h"
#include "videocontroller.h"

#include <QObject>
#include <QtQml>

class PlasmaTube : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(VideoController *videoController READ videoController CONSTANT)
    Q_PROPERTY(SourceManager *sourceManager READ sourceManager CONSTANT)
    Q_PROPERTY(VideoSource *selectedSource READ selectedSource NOTIFY sourceSelected)
    Q_PROPERTY(Settings *settings READ settings CONSTANT)

public:
    static PlasmaTube &instance();

    static PlasmaTube *create(QQmlEngine *, QJSEngine *)
    {
        auto inst = &instance();
        QJSEngine::setObjectOwnership(inst, QJSEngine::ObjectOwnership::CppOwnership);
        return inst;
    }

    VideoController *videoController() const;
    SourceManager *sourceManager() const;
    VideoSource *selectedSource() const;
    QNetworkAccessManager *networkAccessManager() const;

    Q_INVOKABLE void setApplicationProxy();
    Q_INVOKABLE void saveCurrentSource();

    Settings *settings();

Q_SIGNALS:
    void openVideo(const QString &id);
    void errorOccurred(const QString &errorText);
    void finishedLoading();
    void sourceSelected();

private:
    explicit PlasmaTube(QObject *parent = nullptr);

    VideoController *m_controller = nullptr;
    SourceManager *m_sourceManager = nullptr;
    QNetworkAccessManager *m_networkAccessManager = nullptr;

    Settings m_settings;
};
