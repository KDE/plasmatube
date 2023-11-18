// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "config.h"
#include "controllers/plasmatube.h"
#include "controllers/subscriptioncontroller.h"
#include "models/videolistmodel.h"
#include "models/videomodel.h"

#include <QApplication>
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

#include "controllers/windowcontroller.h"
#include "plasmatube-version.h"

std::optional<QString> parseVideoString(const QString &vid)
{
    const QRegularExpression exp(QStringLiteral(R"(https:\/\/[www.]*youtube.com\/watch\?v=(.*))"));
    const auto match = exp.match(vid);
    if (match.hasMatch()) {
        return match.captured(1);
    }

    return std::nullopt;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE") && QQuickStyle::name().isEmpty()) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    KLocalizedString::setApplicationDomain("plasmatube");
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));

    KAboutData about(QStringLiteral("plasmatube"),
                     i18n("PlasmaTube"),
                     QStringLiteral(PLASMATUBE_VERSION_STRING),
                     i18n("YouTube client"),
                     KAboutLicense::GPL_V3,
                     i18n("© Linus Jahn"));
    about.addAuthor(i18n("Linus Jahn"), i18n("Creator"), QStringLiteral("lnj@kaidan.im"));
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

                if (arguments.length() >= 1) {
                    if (const auto videoUrl = parseVideoString(args[0])) {
                        PlasmaTube::instance().openVideo(*videoUrl);
                    }
                }

                return;
            }
        }
    });
#endif

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    PlasmaTubeSettings settings(KSharedConfig::openConfig(QStringLiteral("plasmatuberc"), KConfig::SimpleConfig, QStandardPaths::AppConfigLocation));
    qmlRegisterSingletonInstance<PlasmaTubeSettings>("org.kde.plasmatube.private", 1, 0, "Settings", &settings);
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &settings, &PlasmaTubeSettings::save);

    qmlRegisterType<MpvObject>("org.kde.plasmatube.private", 1, 0, "MpvObject");

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("YouTube client"));

    parser.addPositionalArgument(QStringLiteral("video-url"), QStringLiteral("YouTube video URL to play"));

    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    engine.loadFromModule(QStringLiteral("org.kde.plasmatube"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty())
        return -1;

    if (QApplication::arguments().length() > 1) {
        if (const auto videoUrl = parseVideoString(app.arguments()[1])) {
            PlasmaTube::instance().openVideo(*videoUrl);
        }
    }

#ifdef HAVE_KDBUSADDONS
    QQuickWindow *window = nullptr;

    const auto rootObjects = engine.rootObjects();
    for (auto obj : rootObjects) {
        auto view = qobject_cast<QQuickWindow *>(obj);
        if (view) {
            window = view;
            break;
        }
    }

    if (window != nullptr) {
        auto controller = engine.singletonInstance<WindowController *>(QStringLiteral("org.kde.plasmatube"), QStringLiteral("WindowController"));
        controller->setWindow(window);
        controller->restoreGeometry();
    }
#endif

    return app.exec();
}
