option(BUILD_PRODUCTION "Build for production" OFF)
if(BUILD_PRODUCTION)
    set(BUILD_ENV "Silica")
else(BUILD_PRODUCTION)
    set(BUILD_ENV "Silica-Dev")
endif(BUILD_PRODUCTION)

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

file(GLOB CORE_SRCS
    "GlobalConfig.cpp"
    "MainWindow.cpp"
    "PlayList.cpp"
    "Navigator.cpp"
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
file(GLOB CORE_FORMS
    "*.ui"
    "navigation/*.ui"
    "ui/*.ui"
)

qt5_wrap_ui(CORE_FORMS_HEADERS ${CORE_FORMS})
set(CORE_SRCS ${CORE_SRCS} ${CORE_FORMS_HEADERS})

set(CORE_LINK_LIBS
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
    set(CORE_LINK_LIBS ${CORE_LINK_LIBS} Qt5::OpenGL)
endif(ENABLE_OPENGL)

add_library(silicacore STATIC ${CORE_SRCS})
target_link_libraries(silicacore ${CORE_LINK_LIBS})

# Silica API library
file(GLOB SAPI_SRCS
    "playlist/PlayListProvider.h"
    "sapi/*.cpp"
)
add_definitions(-DSAPI_LIBRARY)
add_library(sapi SHARED ${SAPI_SRCS})
target_link_libraries(sapi silicacore)
set_target_properties(sapi PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)


set(SILICA_SRCS
    "main.cpp"
)
file(GLOB SILICA_RESOURCES "*.qrc")
qt5_add_resources(SILICA_RESOURCES_RCC ${SILICA_RESOURCES})
set(SILICA_SRCS ${SILICA_SRCS} ${SILICA_RESOURCES_RCC})

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

list(APPEND SILICA_SRCS
    "${CMAKE_CURRENT_BINARY_DIR}/Definitions.cpp"
)

if (WIN32)
    add_executable(silica WIN32 ${SILICA_SRCS})
else (WIN32)
    add_executable(silica ${SILICA_SRCS})
endif (WIN32)

target_link_libraries(silica sapi silicacore)

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
install(FILES ${SAPI_PUBLIC_HEADERS} DESTINATION "include/sapi" COMPONENT applications)
install(TARGETS sapi LIBRARY DESTINATION "lib" COMPONENT applications)

if (APPLE)

    # We have to use our own Info.plist template to support Retina display.
    set_target_properties(silica
        PROPERTIES MACOSX_BUNDLE_INFO_PLIST
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/MacOSXBundleInfo.plist.in
    )

    # Do not install executable when building for production, or executable
    # will be packed into `Resources` directory too.
    if (NOT BUILD_PRODUCTION)
        install(TARGETS silica
            BUNDLE DESTINATION .
            RUNTIME DESTINATION .
            COMPONENT applications
        )
    endif()

else ()

    install(TARGETS silica
        RUNTIME DESTINATION bin
        COMPONENT applications
    )

endif()

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins" DESTINATION "lib" COMPONENT applications)

set_target_properties(silica PROPERTIES INSTALL_RPATH "@executable_path;@executable_path/../Resources/lib;$ORIGIN/../lib")


set(CPACK_GENERATOR "TGZ;ZIP")
set(CPACK_PACKAGE_EXECUTABLES "silica;Silica")
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};silica;applications;/")

if (APPLE)
    set(CPACK_GENERATOR "${CPACK_GENERATOR};Bundle")
    set(CPACK_BUNDLE_STARTUP_COMMAND "src/silica")
    set(CPACK_BUNDLE_NAME "silica")
    set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/assets/Info.plist)
    set(CPACK_BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/assets/logo.icns)
endif()

include(CPack)
