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

#include <clocale>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QCommandLineParser>
#include <QQuickStyle>

#include <KLocalizedContext>
#include <KAboutData>
#include <KLocalizedString>

#include "commentsmodel.h"
#include "plasmatube-version.h"

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

    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
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
    qmlRegisterType<CommentsModel>("org.kde.plasmatube.models", 1, 0, "CommentsModel");
    qmlRegisterSingletonType("org.kde.plasmatube", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    PlasmaTubeSettings settings;
    qmlRegisterSingletonInstance<PlasmaTubeSettings>("org.kde.plasmatube", 1, 0, "Settings", &settings);
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &settings, &PlasmaTubeSettings::save);

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("YouTube client"));

    parser.addPositionalArgument(QStringLiteral("video-url"), QStringLiteral("YouTube video URL to play"));

    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    if (QApplication::arguments().length() > 1) {
        auto videoUrl = app.arguments()[1];
        const QRegularExpression exp(QStringLiteral(R"(https:\/\/[www.]*youtube.com\/watch\?v=(.*))"));
        const auto match = exp.match(videoUrl);
        if (match.hasMatch()) {
            PlasmaTube::instance().openVideo(match.captured(1));
        }
    }

    return app.exec();
}
