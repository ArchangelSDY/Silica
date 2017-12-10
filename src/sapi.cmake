# Silica API library
set(SAPI_SRCS
    "sapi/IImageEffectPlugin.cpp"
    "sapi/IImageSource.cpp"
    "sapi/IImageSourceFactory.cpp"
    "sapi/IImageSourcePlugin.cpp"
    "sapi/INavigationPlayer.cpp"
    "sapi/INavigationPlayerPlugin.cpp"
    "sapi/IPlayListProvider.cpp"
    "sapi/IPlayListProviderPlugin.cpp"
    "sapi/ISharerPlugin.cpp"
    "sapi/LoadingIndicator.cpp"
    "sapi/Logging.cpp"
    "sapi/Message.cpp"
    "sapi/NavigatorResource.cpp"
)

add_library(sapi SHARED ${SAPI_SRCS})

target_compile_definitions(sapi PRIVATE -DSAPI_LIBRARY)

target_link_libraries(sapi
    ${ZLIB_LIBRARIES}
    Qt5::Core
)

set_target_properties(sapi PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(SAPI_PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/ImageResource.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/IImageEffectPlugin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/IImageSource.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/IImageSourceFactory.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/IImageSourcePlugin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/INavigationPlayer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/INavigationPlayerPlugin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/IPlayListProvider.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/IPlayListProviderPlugin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/ISharerPlugin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/LoadingIndicator.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/Logging.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/Message.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/NavigatorResource.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/sapi_global.h"
)
install(FILES ${SAPI_PUBLIC_HEADERS} DESTINATION "include/sapi" COMPONENT sdk)
# FIXME: Fix Windows Support
# install(TARGETS sapi LIBRARY DESTINATION "lib" COMPONENT applications)

install(TARGETS sapi
    RUNTIME DESTINATION . COMPONENT applications
    LIBRARY DESTINATION "lib" COMPONENT sdk
    ARCHIVE DESTINATION "lib" COMPONENT sdk
)
