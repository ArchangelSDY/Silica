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
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/Qt7z/Qt7z
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/QtDBMigration/src
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/quazip/quazip
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
    "ui/*.cpp"
    "ui/hotspots/*.cpp"
    "ui/renderers/*.cpp"
)
file(GLOB FORMS "*.ui" "ui/*.ui")
file(GLOB RESOURCES "*.qrc")

qt5_wrap_ui(FORMS_HEADERS ${FORMS})
qt5_add_resources(RESOURCES_RCC ${RESOURCES})
set(SRCS ${SRCS} ${FORMS_HEADERS} ${RESOURCES_RCC})

add_subdirectory(deps/Qt7z)
add_subdirectory(deps/quazip)
add_subdirectory(deps/QtDBMigration)

add_executable(silica ${SRCS})
target_link_libraries(silica
    quazip
    qt7z
    qtdbmigration
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Qt5::Widgets
)

# We have to use our own Info.plist template to support Retina display.
set_target_properties(silica
    PROPERTIES MACOSX_BUNDLE_INFO_PLIST
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/MacOSXBundleInfo.plist.install
)

set(RUNTIME_DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(TARGETS silica BUNDLE DESTINATION ${RUNTIME_DESTINATION} RUNTIME DESTINATION ${RUNTIME_DESTINATION})

# Default config file
set(SILICA_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/assets/Silica.ini)
if (APPLE)
    install(FILES ${SILICA_CONFIG} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/silica.app/Contents/MacOS/config/Asuna)
else ()
    install(FILES ${SILICA_CONFIG} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/config/Asuna)
endif()

# Database migration config
set(SILICA_MIGRATION_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/assets/migration.json)
if (APPLE)
    install(FILES ${SILICA_MIGRATION_CONFIG} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/silica.app/Contents/MacOS)
else ()
    install(FILES ${SILICA_MIGRATION_CONFIG} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

# install(FILES DESTINATION)
# install(TARGETS silica LIBRARY DESTINATION ${LIB_DESTINATION} ARCHIVE DESTINATION ${LIB_DESTINATION} RUNTIME DESTINATION ${LIB_DESTINATION})
