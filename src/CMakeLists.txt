option(ENABLE_OPENGL "Enable OpenGL" OFF)
if(ENABLE_OPENGL)
    add_definitions(-DENABLE_OPENGL)
    find_package(Qt5OpenGL REQUIRED)
    include_directories(${Qt5OpenGL_INCLUDE_DIRS})
endif(ENABLE_OPENGL)

add_definitions(-DQTRAR_STATIC)
add_definitions(-DQUAZIP_STATIC)

add_subdirectory(deps/qwebpa)
add_subdirectory(deps/quazip)
add_subdirectory(deps/Qt7z)
add_subdirectory(deps/QtRAR)
add_subdirectory(deps/QtDBMigration)

# set all include directories for in and out of source builds
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/db
    ${CMAKE_CURRENT_SOURCE_DIR}/image
    ${CMAKE_CURRENT_SOURCE_DIR}/image/caches
    ${CMAKE_CURRENT_SOURCE_DIR}/image/sources
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

set(CMAKE_AUTOMOC ON)

file(GLOB SRCS
    "*.cpp"
    "db/*.cpp"
    "image/*.cpp"
    "image/caches/*.cpp"
    "image/sources/*.cpp"
    "navigation/*.cpp"
    "playlist/*.cpp"
    "playlist/PlayListProvider.h"
    "ui/*.cpp"
    "ui/hotspots/*.cpp"
    "ui/renderers/*.cpp"
)
file(GLOB FORMS
    "*.ui"
    "navigation/*.ui"
    "ui/*.ui"
)
file(GLOB RESOURCES "*.qrc")

qt5_wrap_ui(FORMS_HEADERS ${FORMS})
qt5_add_resources(RESOURCES_RCC ${RESOURCES})
set(SRCS ${SRCS} ${FORMS_HEADERS} ${RESOURCES_RCC})

# Generate compile time definitions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
include(GetGitRevisionDescription)
git_describe(BUILD_GIT_SHA1 "--always")
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Definitions.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Definitions.cpp"
    @ONLY
)
list(APPEND SRCS
    "${CMAKE_CURRENT_BINARY_DIR}/Definitions.cpp"
)

set(LINK_LIBS
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
    ${OpenCV_LIBS}
    qt7z
    qtrar
    qtdbmigration
    quazip
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Qt5::Widgets
)

if(ENABLE_OPENGL)
    set(LINK_LIBS ${LINK_LIBS} Qt5::OpenGL)
endif(ENABLE_OPENGL)


# Silica API library
file(GLOB SAPI_SRCS
    "playlist/PlayListProvider.h"
    "sapi/*.cpp"
)
add_library(sapi SHARED ${SAPI_SRCS})
target_link_libraries(sapi ${LINK_LIBS})


if (WIN32)
    add_executable(silica WIN32 ${SRCS})
else (WIN32)
    add_executable(silica ${SRCS})
endif (WIN32)

target_link_libraries(silica sapi ${LINK_LIBS})

add_custom_command(TARGET silica POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_BINARY_DIR}/deps/qwebpa/src/imageformats
    $<TARGET_FILE_DIR:silica>/plugins/imageformats
)

set(SAPI_PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/IPlayListProvider.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/IPlayListProviderPlugin.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sapi/sapi_global.h"
)
install(FILES ${SAPI_PUBLIC_HEADERS} DESTINATION "include/sapi")
install(TARGETS sapi LIBRARY DESTINATION "lib")

if (APPLE)

    # We have to use our own Info.plist template to support Retina display.
    set_target_properties(silica
        PROPERTIES MACOSX_BUNDLE_INFO_PLIST
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/MacOSXBundleInfo.plist.in
    )

    # Do not install executable if PACK_BUNDLE defined, or executable will
    # be packed into Resources too.
    if (NOT PACK_BUNDLE)
        install(TARGETS silica
            BUNDLE DESTINATION .
            RUNTIME DESTINATION .
        )
    endif()

else ()

    set(RUNTIME_DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(TARGETS silica
        RUNTIME DESTINATION ${RUNTIME_DESTINATION}
    )

endif()

set(SILICA_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/assets/Silica.ini.default)
install(FILES ${SILICA_CONFIG} DESTINATION ./config/Asuna)

set(CPACK_GENERATOR "TGZ;ZIP")
set(CPACK_PACKAGE_EXECUTABLES "silica;Silica")

if (APPLE)
    set(CPACK_GENERATOR "${CPACK_GENERATOR};Bundle")
    set(CPACK_BUNDLE_STARTUP_COMMAND "src/silica")
    set(CPACK_BUNDLE_NAME "silica")
    set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/assets/Info.plist)
    set(CPACK_BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/assets/logo.icns)
endif()

include(CPack)
