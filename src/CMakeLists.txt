# SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
#
# SPDX-License-Identifier: CC0-1.0

add_subdirectory(qinvidious)

set(plasmatube_SRCS
    channelcontroller.cpp
    commentsmodel.cpp
    commentsmodel.h
    main.cpp
    plasmatube.cpp
    logincontroller.cpp
    subscriptionwatcher.cpp
    subscriptioncontroller.cpp
    videomodel.cpp
    videolistmodel.cpp
    playlistsmodel.cpp
    qthelper.hpp
    mpvobject.cpp
    resources.qrc
)

add_executable(plasmatube ${plasmatube_SRCS})

kconfig_add_kcfg_files(plasmatube PlasmaTubeSettings.kcfgc GENERATE_MOC)

target_include_directories(plasmatube PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(plasmatube qinvidious Qt::Core Qt::Qml Qt::Quick Qt::QuickControls2 Qt::Svg Qt::Widgets Qt::Concurrent KF6::CoreAddons KF6::ConfigCore KF6::ConfigGui KF6::I18n ${MPV_LIBRARIES})
target_include_directories(plasmatube PUBLIC ${MPV_INCLUDE_DIRS})
target_compile_options(plasmatube PUBLIC ${MPV_CFLAGS_OTHER} -fexceptions)

if (NOT ANDROID)
    target_link_libraries(plasmatube Qt::DBus)
    target_compile_options(plasmatube PUBLIC -DHAS_DBUS)
endif()

install(TARGETS plasmatube ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
