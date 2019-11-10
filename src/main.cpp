/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mpvobject.h"
#include "videolistmodel.h"
#include "videomodel.h"
#include "accountmanager.h"
#include "invidiousmanager.h"
#include "YTMClient.h"
#include "YTMSearchModel.h"

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
    
    qmlRegisterType<MpvObject>("org.kde.plasmatube.mpv", 1, 0, "MpvObject");

    qmlRegisterType<VideoListModel>("org.kde.plasmatube.models", 1, 0, "VideoListModel");
    qmlRegisterType<VideoModel>("org.kde.plasmatube.models", 1, 0, "VideoModel");

    qmlRegisterType<YTMSearchModel>("org.kde.plasmatube.ytmusic", 1, 0, "SearchModel");
    qmlRegisterSingletonType<YTMClient>("org.kde.plasmatube.ytmusic", 1, 0, "Client", [] (QQmlEngine *, QJSEngine *) {
        return static_cast<QObject*>(YTMClient::instance());
    });

    qmlRegisterUncreatableType<InvidiousManager>("org.kde.plasmatube.invidious", 1, 0, "InvidiousManager", "Only enums defined.");

    qmlRegisterSingletonType<AccountManager>("org.kde.plasmatube.accountmanager", 1, 0, "AccountManager", [] (QQmlEngine *, QJSEngine *) {
        return static_cast<QObject*>(AccountManager::instance());
    });

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
