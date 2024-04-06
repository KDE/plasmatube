# SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
# SPDX-License-Identifier: CC0-1.0

add_subdirectory(api)

add_library(plasmatube_static STATIC)

ecm_qt_declare_logging_category(plasmatube_static
        HEADER "mpris2_logging.h"
        IDENTIFIER "PLASMATUBE_MPRIS2"
        CATEGORY_NAME "org.kde.plasmatube.mpris2"
        DESCRIPTION "PlasmaTube (MPRIS2)"
        DEFAULT_SEVERITY Info
        EXPORT PLASMATUBE
)

ecm_qt_declare_logging_category(plasmatube_static
        HEADER "network_logging.h"
        IDENTIFIER "PLASMATUBE_NETWORK"
        CATEGORY_NAME "org.kde.plasmatube.network"
        DESCRIPTION "PlasmaTube (Network)"
        DEFAULT_SEVERITY Info
        EXPORT PLASMATUBE
)

ecm_qt_declare_logging_category(plasmatube_static
        HEADER "general_logging.h"
        IDENTIFIER "PLASMATUBE_GENERAL"
        CATEGORY_NAME "org.kde.plasmatube"
        DESCRIPTION "PlasmaTube"
        DEFAULT_SEVERITY Info
        EXPORT PLASMATUBE
)

target_sources(plasmatube_static PRIVATE
        utils/linkparser.cpp
        utils/linkparser.h
)
target_link_libraries(plasmatube_static PRIVATE Qt::Core)

add_executable(plasmatube)

qt_add_qml_module(plasmatube
        URI org.kde.plasmatube
        VERSION 1.0)

target_sources(plasmatube PRIVATE
        ${plasmatube_srcs}

        controllers/channelcontroller.cpp
        controllers/channelcontroller.h
        controllers/plasmatube.cpp
        controllers/plasmatube.h
        controllers/logincontroller.cpp
        controllers/logincontroller.h
        controllers/videosource.cpp
        controllers/videosource.h
        controllers/windowcontroller.cpp
        controllers/windowcontroller.h

        models/abstractlistmodel.cpp
        models/abstractlistmodel.h
        models/commentsmodel.cpp
        models/commentsmodel.h
        models/invidiousinstancesmodel.cpp
        models/invidiousinstancesmodel.h
        models/peertubeinstancesmodel.cpp
        models/peertubeinstancesmodel.h
        models/pipedinstancesmodel.cpp
        models/pipedinstancesmodel.h
        models/playlistsmodel.cpp
        models/playlistsmodel.h
        models/searchmodel.cpp
        models/searchmodel.h
        models/videolistmodel.cpp
        models/videolistmodel.h
        models/videomodel.cpp
        models/videomodel.h

        mpris/mpris2.cpp
        mpris/mpris2.h

        controllers/sourcemanager.cpp
        controllers/sourcemanager.h
        controllers/subscriptioncontroller.cpp
        controllers/subscriptioncontroller.h
        controllers/subscriptionwatcher.cpp
        controllers/subscriptionwatcher.h
        controllers/videoqueue.cpp
        controllers/videoqueue.h
        controllers/videocontroller.cpp
        controllers/videocontroller.h

        utils/mpvobject.cpp
        utils/mpvobject.h

        main.cpp
)

if (NOT ANDROID)
    target_sources(plasmatube PRIVATE
            mpris/mediaplayer2.h
            mpris/mediaplayer2.cpp
            mpris/mediaplayer2player.cpp
            mpris/mediaplayer2player.h
            mpris/metadata.cpp
            mpris/metadata.h
    )
endif()

qt_target_qml_sources(plasmatube
    QML_FILES
        ui/components/BaseGridItem.qml
        ui/components/BaseGridView.qml
        ui/components/BottomNavBar.qml
        ui/components/ChannelGridItem.qml
        ui/components/Comments.qml
        ui/components/PlaceholderItem.qml
        ui/components/PlaylistGridItem.qml
        ui/components/ShareAction.qml
        ui/components/ShareDialog.qml
        ui/components/ShareMenu.qml
        ui/components/Sidebar.qml
        ui/components/SourceSwitcher.qml
        ui/components/SubscriptionButton.qml
        ui/components/VideoGridItem.qml
        ui/components/VideoListAction.qml
        ui/components/VideoListItem.qml
        ui/components/VideoMenu.qml

        ui/loginflow/AddInvidiousPage.qml
        ui/loginflow/AddPeerTubePage.qml
        ui/loginflow/AddPipedPage.qml
        ui/loginflow/WelcomePage.qml

        ui/settings/AccountCard.qml
        ui/settings/BaseSourcePage.qml
        ui/settings/GeneralPage.qml
        ui/settings/InvidiousSourcePage.qml
        ui/settings/NetworkProxyPage.qml
        ui/settings/PeerTubeSourcePage.qml
        ui/settings/PipedSourcePage.qml
        ui/settings/SettingsPage.qml
        ui/settings/SourcesPage.qml

        ui/videoplayer/MinimizedVideoPlayer.qml
        ui/videoplayer/PictureInPictureVideo.qml
        ui/videoplayer/TabIndicator.qml
        ui/videoplayer/VideoControls.qml
        ui/videoplayer/VideoData.qml
        ui/videoplayer/VideoPlayer.qml
        ui/videoplayer/VideoPlayerParent.qml
        ui/videoplayer/VideoQueueView.qml

        ui/ChannelPage.qml
        ui/FeedPage.qml
        ui/HistoryPage.qml
        ui/LoginPage.qml
        ui/Main.qml
        ui/PlaylistPage.qml
        ui/PlaylistsPage.qml
        ui/SearchPage.qml
        ui/SubscriptionsPage.qml
        ui/TrendingPage.qml
    RESOURCES
        ui/components/utils.js
)

set_source_files_properties(../org.kde.plasmatube.svg PROPERTIES
        QT_RESOURCE_ALIAS /org.kde.plasmatube.svg
)

qt_target_qml_sources(plasmatube
        PREFIX /
        RESOURCES
        ../org.kde.plasmatube.svg
)

kconfig_add_kcfg_files(plasmatube config.kcfgc sourceconfig.kcfgc GENERATE_MOC)

target_include_directories(plasmatube PRIVATE
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/controllers
        ${CMAKE_CURRENT_SOURCE_DIR}/models
        ${CMAKE_CURRENT_SOURCE_DIR}/mpris
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
)
target_link_libraries(plasmatube PRIVATE
        plasmatube_static
        qinvidious
        qinvidiousplugin
        Qt::Qml
        Qt::Quick
        Qt::QuickControls2
        Qt::Concurrent
        KF6::CoreAddons
        KF6::ConfigCore
        KF6::ConfigGui
        KF6::I18n
        Qt6Keychain::Qt6Keychain
        MpvQt::MpvQt)

if (NOT ANDROID)
    target_link_libraries(plasmatube PRIVATE Qt::DBus Qt::Widgets)
    target_compile_options(plasmatube PUBLIC -DHAS_DBUS)
else()
    target_link_libraries(plasmatube PRIVATE Qt::Svg)
endif()

if (TARGET KF6::DBusAddons)
    target_link_libraries(plasmatube PRIVATE KF6::DBusAddons KF6::WindowSystem)
    target_compile_definitions(plasmatube PRIVATE -DHAVE_KDBUSADDONS)
endif ()

if(PLASMATUBE_FLATPAK)
    target_compile_definitions(plasmatube PUBLIC PLASMATUBE_FLATPAK)
endif()

if (NOT ANDROID)
    install(DIRECTORY icons/ DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/actions)
else()
    install(DIRECTORY icons/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami.2/icons/)
endif()

install(TARGETS plasmatube ${KF6_INSTALL_TARGETS_DEFAULT_ARGS})

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
