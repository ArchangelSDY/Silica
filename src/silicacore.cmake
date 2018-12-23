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
    "image/Image.cpp"
    "image/ImageHistogram.cpp"
    "image/ImageHotspot.cpp"
    "image/ImageRank.cpp"
    "image/ImageSource.cpp"
    "image/ImageSourceFactory.cpp"
    "image/ImageSourceManager.cpp"
    "image/ImageThumbnailBlockLoader.cpp"
    "image/caches/*.cpp"
    "image/effects/ImageEffectManager.cpp"
    "image/metadata/ImageMetadataConstants.cpp"
    "image/sources/LocalImageSource.cpp"
    "image/sources/LocalImageSourceFactory.cpp"
    "image/sources/RARImageSource.cpp"
    "image/sources/RARImageSourceFactory.cpp"
    "image/sources/SevenzImageSource.cpp"
    "image/sources/SevenzImageSourceFactory.cpp"
    "image/sources/ZipImageSource.cpp"
    "image/sources/ZipImageSourceFactory.cpp"
    "logger/*.cpp"
    "logger/listeners/*.cpp"
    "navigation/AbstractNavigationPlayer.cpp"
    "navigation/CascadeClassifierNavigationPlayer.cpp"
    "navigation/ExpandingNavigationPlayer.cpp"
    "navigation/FixedRegionConfDialog.cpp"
    "navigation/FixedRegionNavigationPlayer.cpp"
    "navigation/HotspotsNavigationPlayer.cpp"
    "navigation/MangaNavigationPlayer.cpp"
    "navigation/NavigationPlayerManager.cpp"
    "navigation/NavigatorSynchronizer.cpp"
    "navigation/NormalNavigationPlayer.cpp"
    "playlist/group/AbstractPlayListGrouper.cpp"
    "playlist/group/PlayListImageMetadataGrouper.cpp"
    "playlist/group/PlayListImageThumbnailHistogramGrouper.cpp"
    "playlist/sort/PlayListImageAspectRatioSorter.cpp"
    "playlist/sort/PlayListImageNameSorter.cpp"
    "playlist/sort/PlayListImageSizeSorter.cpp"
    "playlist/sort/PlayListImageUrlSorter.cpp"
    "playlist/sort/Utils.cpp"
    "playlist/AbstractPlayListFilter.cpp"
    "playlist/DoNothingFilter.cpp"
    "playlist/EqualRankFilter.cpp"
    "playlist/LocalPlayListProvider.cpp"
    "playlist/LocalPlayListProviderFactory.cpp"
    "playlist/MinRankFilter.cpp"
    "playlist/NotEqualRankFilter.cpp"
    "playlist/PlayListProvider.h"
    "playlist/PlayListProviderFactory.cpp"
    "playlist/PlayListProviderManager.cpp"
    "playlist/PlayListRecord.cpp"
    "share/Sharer.h"
    "share/SharerManager.cpp"
    "sapi/ImageDelegate.cpp"
    "sapi/ImageEffectDelegate.cpp"
    "sapi/ImageSourceDelegate.cpp"
    "sapi/ImageSourceFactoryDelegate.cpp"
    "sapi/LoadingIndicatorDelegate.cpp"
    "sapi/NavigatorDelegate.cpp"
    "sapi/NavigationPlayerDelegate.cpp"
    "sapi/PlayListProviderDelegate.cpp"
    "sapi/PlayListProviderFactoryDelegate.cpp"
    "sapi/PluginLoggingDelegate.cpp"
    "sapi/SharerPluginDelegate.cpp"
    "ui/models/MainGraphicsViewModel.cpp"
    "ui/models/TaskProgress.cpp"
    "util/CvUtils.cpp"
)

if(WIN32)
    set(CORE_SRCS ${CORE_SRCS}
        "keychain/KeyChain_Win.cpp"
    )
else()
    set(CORE_SRCS ${CORE_SRCS}
        "keychain/KeyChain.cpp"
    )
endif()

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
    quazip_static
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
