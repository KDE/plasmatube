# SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
#
# SPDX-License-Identifier: CC0-1.0

add_subdirectory(qinvidious)

add_executable(plasmatube)

qt_add_qml_module(plasmatube
        URI org.kde.plasmatube
        VERSION 1.0)

target_sources(plasmatube PRIVATE
        channelcontroller.cpp
        channelcontroller.h
        commentsmodel.cpp
        commentsmodel.h
        invidiousinstancesmodel.cpp
        invidiousinstancesmodel.h
        logincontroller.cpp
        logincontroller.h
        main.cpp
        mpvobject.cpp
        mpvobject.h
        peertubeinstancesmodel.cpp
        peertubeinstancesmodel.h
        plasmatube.cpp
        plasmatube.h
        playlistsmodel.cpp
        playlistsmodel.h
        qthelper.hpp
        sourcemanager.cpp
        sourcemanager.h
        subscriptioncontroller.cpp
        subscriptioncontroller.h
        subscriptionwatcher.cpp
        subscriptionwatcher.h
        videocontroller.cpp
        videocontroller.h
        videolistmodel.cpp
        videolistmodel.h
        videomodel.cpp
        videomodel.h
        videosource.cpp
        videosource.h
)

qt_target_qml_sources(plasmatube
    QML_FILES
        contents/ui/components/BottomNavBar.qml
        contents/ui/components/Comments.qml
        contents/ui/components/ShareDialog.qml
        contents/ui/components/ShareMenu.qml
        contents/ui/components/Sidebar.qml
        contents/ui/components/SubscriptionButton.qml

        contents/ui/loginflow/AddInvidiousPage.qml
        contents/ui/loginflow/AddPeerTubePage.qml
        contents/ui/loginflow/WelcomePage.qml

        contents/ui/videoplayer/MinimizedVideoPlayer.qml
        contents/ui/videoplayer/PictureInPictureVideo.qml
        contents/ui/videoplayer/TabIndicator.qml
        contents/ui/videoplayer/VideoControls.qml
        contents/ui/videoplayer/VideoData.qml
        contents/ui/videoplayer/VideoPlayer.qml
        contents/ui/videoplayer/VideoPlayerParent.qml

        contents/ui/ChannelPage.qml
        contents/ui/FeedPage.qml
        contents/ui/HistoryPage.qml
        contents/ui/InvidiousSourcePage.qml
        contents/ui/LoginPage.qml
        contents/ui/Main.qml
        contents/ui/NetworkProxyPage.qml
        contents/ui/PeerTubeSourcePage.qml
        contents/ui/PlaylistGridItem.qml
        contents/ui/PlaylistPage.qml
        contents/ui/PlaylistsPage.qml
        contents/ui/PopularPage.qml
        contents/ui/SearchPage.qml
        contents/ui/SettingsPage.qml
        contents/ui/SubscriptionsPage.qml
        contents/ui/SourceSwitcher.qml
        contents/ui/TrendingPage.qml
        contents/ui/VideoGridItem.qml
        contents/ui/VideoListAction.qml
        contents/ui/VideoListItem.qml
        contents/ui/VideoMenu.qml
    RESOURCES
        contents/ui/utils.js
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

target_include_directories(plasmatube PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(plasmatube PRIVATE qinvidious qinvidiousplugin Qt::Core Qt::Qml Qt::Quick Qt::QuickControls2 Qt::Svg Qt::Widgets Qt::Concurrent KF6::CoreAddons KF6::ConfigCore KF6::ConfigGui KF6::I18n Qt6Keychain::Qt6Keychain ${MPV_LIBRARIES})
target_include_directories(plasmatube PUBLIC ${MPV_INCLUDE_DIRS})
target_compile_options(plasmatube PUBLIC ${MPV_CFLAGS_OTHER} -fexceptions)

if (NOT ANDROID)
    target_link_libraries(plasmatube PRIVATE Qt::DBus)
    target_compile_options(plasmatube PUBLIC -DHAS_DBUS)
endif()

install(TARGETS plasmatube ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
