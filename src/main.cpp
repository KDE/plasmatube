// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "config.h"
#include "controllers/plasmatube.h"
#include "controllers/subscriptioncontroller.h"
#include "models/videolistmodel.h"
#include "utils/linkparser.h"

#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif
#include <QCommandLineParser>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQuickWindow>

#include <KLocalizedContext>
#include <KAboutData>
#include <KLocalizedString>

#ifdef HAVE_KDBUSADDONS
#include <KDBusService>
#include <KWindowSystem>
#endif

#include "plasmatube-version.h"

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char **argv)
{
#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
#else
    QApplication app(argc, argv);

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#endif

    KLocalizedString::setApplicationDomain("plasmatube");
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));

    KAboutData about(QStringLiteral("plasmatube"),
                     i18n("PlasmaTube"),
                     QStringLiteral(PLASMATUBE_VERSION_STRING),
                     i18n("YouTube client"),
                     KAboutLicense::GPL_V3,
                     i18n("© 2019-2024 Linus Jahn, 2019-2024 KDE Community"));
    about.addAuthor(i18n("Linus Jahn"), i18n("Creator"), QStringLiteral("lnj@kaidan.im"));
    about.addAuthor(i18n("Joshua Goins"),
                    i18n("Maintainer"),
                    QStringLiteral("josh@redstrate.com"),
                    QStringLiteral("https://redstrate.com/"),
                    QUrl(QStringLiteral("https://redstrate.com/rss-image.png")));
    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    about.setOrganizationDomain("kde.org");
    about.setBugAddress("https://bugs.kde.org/describecomponents.cgi?product=plasmatube");

    KAboutData::setApplicationData(about);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.plasmatube")));

    QQmlApplicationEngine engine;

#ifdef HAVE_KDBUSADDONS
    KDBusService service(KDBusService::Unique);
    QObject::connect(&service, &KDBusService::activateRequested, &engine, [&engine](const QStringList &arguments, const QString & /*workingDirectory*/) {
        const auto rootObjects = engine.rootObjects();
        for (auto obj : rootObjects) {
            if (auto view = qobject_cast<QQuickWindow *>(obj)) {
                KWindowSystem::updateStartupId(view);
                KWindowSystem::activateWindow(view);

                if (arguments.isEmpty()) {
                    return;
                }

                auto args = arguments;
                args.removeFirst();

                if (!arguments.empty()) {
                    if (const auto videoUrl = LinkParser::parseVideoString(args[0])) {
                        PlasmaTube::instance().openVideo(*videoUrl);
                    }
                }

                return;
            }
        }
    });
#endif

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("video-url"), QStringLiteral("YouTube video URL to play"));

    about.setupCommandLine(&parser);
    parser.process(app);

    engine.loadFromModule(QStringLiteral("org.kde.plasmatube"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty())
        return -1;

    if (!parser.positionalArguments().empty()) {
        if (const auto videoUrl = LinkParser::parseVideoString(parser.positionalArguments().first())) {
            PlasmaTube::instance().openVideo(*videoUrl);
        }
    }

    return QCoreApplication::exec();
}
