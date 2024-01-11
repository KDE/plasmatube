# SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required(VERSION 3.16)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "24")
set(RELEASE_SERVICE_VERSION_MINOR "01")
set(RELEASE_SERVICE_VERSION_MICRO "95")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(plasmatube VERSION ${RELEASE_SERVICE_VERSION})

set(KF_MIN_VERSION "5.240.0")
set(QT_MIN_VERSION "6.5.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(ECMSetupVersion)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(FeatureSummary)
include(ECMFindQmlModule)
include(ECMQtDeclareLoggingCategory)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Quick Test Gui Svg QuickControls2 Widgets Concurrent)
qt_policy(SET QTP0001 NEW)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS CoreAddons Config Kirigami I18n)
find_package(KF6KirigamiAddons 0.11 REQUIRED)
find_package(MpvQt REQUIRED)

find_package(Qt6Keychain)
set_package_properties(Qt6Keychain PROPERTIES
        TYPE REQUIRED
        PURPOSE "Secure storage of account secrets"
)

ecm_find_qmlmodule(org.kde.purpose 1.0)

if (NOT ANDROID)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS DBus)
endif()

if (NOT ANDROID AND NOT WIN32 AND NOT APPLE)
    find_package(KF6DBusAddons ${KF_MIN_VERSION} REQUIRED)
    find_package(KF6WindowSystem ${KF_MIN_VERSION} REQUIRED)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX PLASMATUBE
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/plasmatube-version.h
)

add_subdirectory(src)

ki18n_install(po)

install(FILES org.kde.plasmatube.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.plasmatube.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.plasmatube.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
