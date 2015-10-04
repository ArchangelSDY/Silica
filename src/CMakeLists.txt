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

add_subdirectory(deps/qwebpa)
add_subdirectory(deps/quazip)
add_subdirectory(deps/Qt7z)
add_subdirectory(deps/QtRAR)
add_subdirectory(deps/QtDBMigration)

include("sapi.cmake")
include("silicacore.cmake")
include("silica.cmake")


set(CPACK_GENERATOR "TGZ;ZIP")
set(CPACK_PACKAGE_EXECUTABLES "silica;Silica")
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};silica;applications;/")
set(CPACK_PACKAGE_VERSION_MAJOR ${silica_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${silica_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${silica_VERSION_PATCH})

if (APPLE)
    set(CPACK_GENERATOR "${CPACK_GENERATOR};Bundle")
    set(CPACK_BUNDLE_STARTUP_COMMAND "src/silica")
    set(CPACK_BUNDLE_NAME "silica")
    set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/assets/Info.plist)
    set(CPACK_BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/assets/logo.icns)
endif()

include(CPack)
