#include "mpvobject.h"

#include <clocale>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char **argv)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QCoreApplication::setOrganizationName("KDE");
    QCoreApplication::setOrganizationDomain("kde.org");
    QCoreApplication::setApplicationName("plasmatube");
    QGuiApplication::setApplicationDisplayName("PlasmaTube");

    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
    std::setlocale(LC_NUMERIC, "C");

    qmlRegisterType<MpvObject>("org.kde.plasmatube", 1, 0, "MpvObject");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
