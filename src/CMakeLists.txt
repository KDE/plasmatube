set(plasmatube_SRCS
    main.cpp
    mpvobject.cpp
    videomodel.cpp
    videolistmodel.cpp
    videobasicinfo.cpp
    video.cpp
    MediaFormat.cpp
    invidiousmanager.cpp
    accountmanager.cpp

    ytmusic/YTMApiRequest.cpp
    ytmusic/YTMSearchRequest.cpp
    ytmusic/YTMSearchResult.cpp
    ytmusic/YTMThumbnail.cpp
    ytmusic/YTMThumbnailSet.cpp

    ytmusic/YTMClient.cpp
    ytmusic/YTMSearchModel.cpp
)

qt5_add_resources(RESOURCES resources.qrc)
add_executable(plasmatube ${plasmatube_SRCS} ${RESOURCES})
target_link_libraries(plasmatube Qt5::Core Qt5::Qml Qt5::Quick Qt5::Svg ${MPV_LIBRARIES})
target_include_directories(plasmatube PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/ytmusic
    ${MPV_INCLUDE_DIRS}
)
target_compile_options(plasmatube PUBLIC ${MPV_CFLAGS_OTHER} -fexceptions)

install(TARGETS plasmatube ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
