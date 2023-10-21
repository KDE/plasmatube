// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>
#include <optional>

#include "sourcemanager.h"
#include "videocontroller.h"

class PlasmaTube : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(VideoController *videoController READ videoController CONSTANT)
    Q_PROPERTY(SourceManager *sourceManager READ sourceManager CONSTANT)

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

    std::optional<bool> isSubscribedToChannel(const QString &jid) const;

    void fetchSubscriptions();

    Q_INVOKABLE bool isVideoWatched(const QString &videoId);
    Q_INVOKABLE void markVideoWatched(const QString &videoId);
    void markVideoUnwatched(const QString &videoId);

    Q_INVOKABLE void setInhibitSleep(bool inhibit);

    Q_INVOKABLE void addToPlaylist(const QString &plid, const QString &videoId);

Q_SIGNALS:
    void subscriptionsChanged();
    void openVideo(const QString &id);
    void errorOccurred(const QString &errorText);
    void finishedLoading();

private:
    explicit PlasmaTube(QObject *parent = nullptr);

    friend class SubscriptionController;

    void setSubscriptions(const QList<QString> &subscriptions);
    std::optional<QList<QString>> &subscriptions();

    void fetchHistory(qint32 page = 1);

    std::optional<QList<QString>> m_subscriptions;
    QList<QString> m_watchedVideos;
    unsigned int screenSaverDbusCookie = 0;
    VideoController *m_controller = nullptr;
    SourceManager *m_sourceManager = nullptr;
};
