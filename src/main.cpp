// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PlasmaTubeSettings.h"
#include "logincontroller.h"
#include "plasmatube.h"
#include "subscriptioncontroller.h"
#include "videolistmodel.h"
#include "videomodel.h"
#include "mpvobject.h"

#include "qinvidious/invidiousapi.h"
#include "qinvidious/searchparameters.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <KLocalizedContext>

int main(int argc, char **argv)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("KDE");
    QCoreApplication::setOrganizationDomain("kde.org");
    QCoreApplication::setApplicationName("plasmatube");
    QGuiApplication::setApplicationDisplayName("PlasmaTube");

    // required by mpv
    setlocale(LC_NUMERIC, "C");

    qmlRegisterType<MpvObject>("org.kde.plasmatube", 1, 0, "MpvObject");
    qmlRegisterType<VideoListModel>("org.kde.plasmatube.models", 1, 0, "VideoListModel");
    qmlRegisterType<VideoModel>("org.kde.plasmatube.models", 1, 0, "VideoModel");
    qmlRegisterType<LogInController>("org.kde.plasmatube", 1, 0, "LogInController");
    qmlRegisterType<SubscriptionWatcher>("org.kde.plasmatube", 1, 0, "SubscriptionWatcher");
    qmlRegisterType<SubscriptionController>("org.kde.plasmatube", 1, 0, "SubscriptionController");
    qmlRegisterSingletonInstance<PlasmaTube>("org.kde.plasmatube", 1, 0, "PlasmaTube", &PlasmaTube::instance());
    qRegisterMetaType<SearchParameters::SortBy>("SortBy");
    qRegisterMetaType<SearchParameters::Date>("Date");
    qRegisterMetaType<SearchParameters::Duration>("Duration");
    qRegisterMetaType<SearchParameters::Type>("Type");
    qRegisterMetaType<SearchParameters::Feature>("Feature");
    qmlRegisterType<SearchParameters>("org.kde.plasmatube", 1, 0, "SearchParameters");
    qRegisterMetaType<SearchParameters*>("const SearchParameters*");

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    PlasmaTubeSettings settings;
    qmlRegisterSingletonInstance<PlasmaTubeSettings>("org.kde.plasmatube", 1, 0, "Settings", &settings);
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &settings, &PlasmaTubeSettings::save);

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
