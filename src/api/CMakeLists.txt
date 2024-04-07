# SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
# SPDX-License-Identifier: CC0-1.0

add_library(qinvidious STATIC)
ecm_add_qml_module(qinvidious
        OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/org/kde/plasmatube/invidious"
        URI org.kde.plasmatube.invidious
        VERSION 1.0
        GENERATE_PLUGIN_SOURCE)

target_sources(qinvidious PRIVATE
        invidious/invidiousapi.cpp
        invidious/invidiousapi.h

        peertube/peertubeapi.cpp
        peertube/peertubeapi.h

        piped/pipedapi.cpp
        piped/pipedapi.h

        abstractapi.h
        abstractapi.cpp
        caption.cpp
        caption.h
        channel.cpp
        channel.h
        comment.cpp
        comment.h
        mediaformat.cpp
        mediaformat.h
        paginator.cpp
        paginator.h
        playlist.cpp
        playlist.h
        preferences.cpp
        preferences.h
        qinvidiousglobal.h
        searchparameters.cpp
        searchparameters.h
        searchresult.cpp
        searchresult.h
        video.cpp
        video.h
        videobasicinfo.cpp
        videobasicinfo.h
        videothumbnail.cpp
        videothumbnail.h
)

target_link_libraries(qinvidious PRIVATE Qt::Core Qt::Network KF6::I18n Qt6Keychain::Qt6Keychain)

if(PLASMATUBE_FLATPAK)
    target_compile_definitions(qinvidious PUBLIC PLASMATUBE_FLATPAK)
endif()
