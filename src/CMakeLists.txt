# SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
#
# SPDX-License-Identifier: CC0-1.0

add_subdirectory(qinvidious)

set(plasmatube_SRCS
    main.cpp
    plasmatube.cpp
    logincontroller.cpp
    subscriptionwatcher.cpp
    subscriptioncontroller.cpp
    videomodel.cpp
    videolistmodel.cpp
)

qt5_add_resources(RESOURCES resources.qrc)

add_executable(plasmatube ${plasmatube_SRCS} ${RESOURCES})

kconfig_add_kcfg_files(plasmatube PlasmaTubeSettings.kcfgc GENERATE_MOC)

target_include_directories(plasmatube PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(plasmatube qinvidious Qt5::Core Qt5::Qml Qt5::Quick Qt5::Svg KF5::ConfigCore KF5::ConfigGui KF5::I18n)

install(TARGETS plasmatube ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
