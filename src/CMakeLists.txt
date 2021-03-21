# SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
#
# SPDX-License-Identifier: CC0-1.0

set(qinvidious_SRCS
    qinvidious/videobasicinfo.cpp
    qinvidious/video.cpp
)

set(plasmatube_SRCS
    main.cpp
    videomodel.cpp
    videolistmodel.cpp
    invidiousmanager.cpp
    accountmanager.cpp
)

qt5_add_resources(RESOURCES resources.qrc)
add_executable(plasmatube ${qinvidious_SRCS} ${plasmatube_SRCS} ${RESOURCES})
target_link_libraries(plasmatube Qt5::Core Qt5::Qml Qt5::Quick Qt5::Svg KF5::I18n)

install(TARGETS plasmatube ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
