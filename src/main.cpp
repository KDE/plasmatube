// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videolistmodel.h"
#include "videomodel.h"
#include "accountmanager.h"
#include "invidiousmanager.h"

#include <clocale>

#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char **argv)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("KDE");
    QCoreApplication::setOrganizationDomain("kde.org");
    QCoreApplication::setApplicationName("plasmatube");
    QGuiApplication::setApplicationDisplayName("PlasmaTube");

    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
    std::setlocale(LC_NUMERIC, "C");

    qmlRegisterType<VideoListModel>("org.kde.plasmatube.models", 1, 0, "VideoListModel");
    qmlRegisterType<VideoModel>("org.kde.plasmatube.models", 1, 0, "VideoModel");
    qmlRegisterUncreatableType<InvidiousManager>("org.kde.plasmatube.invidious", 1, 0, "InvidiousManager", "Only enums defined.");
    qmlRegisterSingletonInstance<AccountManager>("org.kde.plasmatube.accountmanager", 1, 0, "AccountManager", &AccountManager::instance());

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
