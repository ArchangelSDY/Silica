# set all include directories for in and out of source builds
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/db
    ${CMAKE_CURRENT_SOURCE_DIR}/image
    ${CMAKE_CURRENT_SOURCE_DIR}/image/caches
    ${CMAKE_CURRENT_SOURCE_DIR}/image/sources
    ${CMAKE_CURRENT_SOURCE_DIR}/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/listeners
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation
    ${CMAKE_CURRENT_SOURCE_DIR}/playlist
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/hotspots
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/renderers
    ${CMAKE_CURRENT_SOURCE_DIR}/util
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/quazip/quazip
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/Qt7z/Qt7z
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/QtDBMigration/src
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/QtRAR/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

file(GLOB CORE_SRCS
    "GlobalConfig.cpp"
    "PlayList.cpp"
    "Navigator.cpp"
    "db/*.cpp"
    "image/*.cpp"
    "image/caches/*.cpp"
    "image/sources/*.cpp"
    "logger/*.cpp"
    "logger/listeners/*.cpp"
    "navigation/*.cpp"
    "playlist/*.cpp"
    "playlist/PlayListProvider.h"
    "share/*.cpp"
    "share/Sharer.h"
    "sapi/ImageDelegate.cpp"
    "sapi/LoadingIndicatorDelegate.cpp"
    "sapi/PlayListProviderDelegate.cpp"
    "sapi/PlayListProviderFactoryDelegate.cpp"
    "sapi/PluginLoggingDelegate.cpp"
    "ui/models/MainGraphicsViewModel.cpp"
    "ui/models/TaskProgress.cpp"
)

qt5_wrap_ui(CORE_UI_HEADERS
    "navigation/FixedRegionConfDialog.ui"
)

set(CORE_LINK_LIBS
    sapi
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
    ${OpenCV_LIBS}
    qt7z
    qtrar
    qtdbmigration
    quazip
    Qt5::Concurrent
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Qt5::Widgets
)

if(ENABLE_OPENGL)
    set(CORE_LINK_LIBS ${CORE_LINK_LIBS} Qt5::OpenGL)
endif(ENABLE_OPENGL)

add_library(silicacoreobjs STATIC
    ${CORE_SRCS}
    ${CORE_UI_HEADERS}
)
target_link_libraries(silicacoreobjs ${CORE_LINK_LIBS})

add_library(silicacore STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/Definitions.cpp
)
target_link_libraries(silicacore silicacoreobjs)
