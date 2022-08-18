# SPDX-FileCopyrightText: 2021 Linus Jahn <lnj@kaidan.im>
# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: CC0-1.0

set(qinvidious_SRCS
    invidiousapi.cpp
    credentials.cpp
    videobasicinfo.cpp
    video.cpp
    videothumbnail.cpp
    caption.cpp
    mediaformat.cpp
    searchparameters.cpp
)

add_library(qinvidious STATIC ${qinvidious_SRCS})
target_link_libraries(qinvidious Qt::Core Qt::Network)
