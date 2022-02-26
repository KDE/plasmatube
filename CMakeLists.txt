# SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
#
# SPDX-License-Identifier: CC0-1.0

project(plasmatube)

cmake_minimum_required(VERSION 3.16)
set(KF5_MIN_VERSION "5.81.0")
set(QT_MIN_VERSION "5.15.0")

################# Disallow in-source build #################

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "This application requires an out of source build. Please create a separate build directory.")
endif()

include(FeatureSummary)

################# set KDE specific information #################

find_package(ECM 0.0.8 REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMPoQmTools)
include(KDECompilerSettings NO_POLICY_SCOPE)

################# Find dependencies #################

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Quick Test Gui Svg QuickControls2)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Kirigami2 I18n)

################# Enable C++17 #################

# C++ standard settings:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

################# build and install #################
add_subdirectory(src)

install(PROGRAMS org.kde.plasmatube.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.plasmatube.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.plasmatube.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
