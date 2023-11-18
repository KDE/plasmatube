# SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: CC0-1.0

qt_add_qml_module(qinvidious
        STATIC
        URI org.kde.plasmatube.invidious
        VERSION 1.0)

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
        credentials.cpp
        credentials.h
        mediaformat.cpp
        mediaformat.h
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

target_link_libraries(qinvidious PRIVATE Qt::Core Qt::Network KF6::I18n)
