# Silica API library
file(GLOB SAPI_SRCS
    "sapi/IPlayListProvider.cpp"
    "sapi/IPlayListProviderPlugin.cpp"
    "sapi/ISharerPlugin.cpp"
    "sapi/LoadingIndicator.cpp"
    "sapi/Logging.cpp"
)

add_definitions(-DSAPI_LIBRARY)
add_library(sapi SHARED ${SAPI_SRCS})

target_link_libraries(sapi
    Qt5::Core
)

set_target_properties(sapi PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(SAPI_PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/ImageResource.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/IPlayListProvider.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/IPlayListProviderPlugin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/ISharerPlugin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/LoadingIndicator.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/Logging.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/sapi_global.h"
)
install(FILES ${SAPI_PUBLIC_HEADERS} DESTINATION "include/sapi" COMPONENT applications)
install(TARGETS sapi LIBRARY DESTINATION "lib" COMPONENT applications)
