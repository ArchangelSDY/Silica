if(BUILD_PRODUCTION)
    set(BUILD_ENV "Silica")
else(BUILD_PRODUCTION)
    set(BUILD_ENV "Silica-Dev")
endif(BUILD_PRODUCTION)

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

set(DEFINITION_SRCS
    "${CMAKE_CURRENT_BINARY_DIR}/Definitions.cpp"
)

add_definitions(-DQTRAR_STATIC)
add_definitions(-DQUAZIP_STATIC)
add_definitions(-DQT7Z_STATIC)

add_subdirectory(deps/qwebpa)
add_subdirectory(deps/quazip)
add_subdirectory(deps/Qt7z)
add_subdirectory(deps/QtRAR)
add_subdirectory(deps/QtDBMigration)

include("sapi.cmake")
include("silicacore.cmake")
include("silicaui.cmake")
include("silica.cmake")

set(CPACK_GENERATOR "TGZ;ZIP")
# TODO: This causes components packaging failure
# set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};Silica;applications;applications")
set(CPACK_PACKAGE_EXECUTABLES "silica;Silica")
set(CPACK_PACKAGE_VERSION_MAJOR ${silica_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${silica_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${silica_VERSION_PATCH})
set(CPACK_COMPONENTS_ALL applications sdk)
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION "Silica Main Application")
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Silica")
set(CPACK_COMPONENT_SDK_DESCRIPTION "Silica SDK For Plugin Development")
set(CPACK_COMPONENT_SDK_DISPLAY_NAME "Silica SDK")
set(CPACK_COMPONENT_SDK_DEPENDS applications)

if (APPLE)
    set(CPACK_GENERATOR "${CPACK_GENERATOR};Bundle")
    set(CPACK_BUNDLE_STARTUP_COMMAND "src/silica")
    set(CPACK_BUNDLE_NAME "silica")
    set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/assets/Info.plist)
    set(CPACK_BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/assets/logo.icns)
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS64")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Silica")
endif()

include(CPack)
