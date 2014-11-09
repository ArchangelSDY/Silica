# set all include directories for in and out of source builds
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/QtMockWebServer
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/db
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/image
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/playlist
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/db
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/image
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/image/caches
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/image/sources
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/navigation
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/playlist
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/hotspots
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/renderers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/util
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/deps/Qt7z/Qt7z
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/deps/QtDBMigration/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/deps/quazip/quazip
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(CMAKE_AUTOMOC ON)

file(GLOB SRCS
    "../src/GlobalConfig.cpp"
    "../src/MainWindow.cpp"
    "../src/Navigator.cpp"
    "../src/PlayList.cpp"
    "../src/db/*.cpp"
    "../src/image/*.cpp"
    "../src/image/caches/*.cpp"
    "../src/image/sources/*.cpp"
    "../src/navigation/*.cpp"
    "../src/playlist/*.cpp"
    "../src/ui/*.cpp"
    "../src/ui/hotspots/*.cpp"
    "../src/ui/renderers/*.cpp"
    "*.cpp"
    "utils/*.cpp"
)

file(GLOB FORMS "../src/*.ui" "../src/ui/*.ui")
file(GLOB RESOURCES "../src/*.qrc")

qt5_wrap_ui(FORMS_HEADERS ${FORMS})
qt5_add_resources(RESOURCES_RCC ${RESOURCES})
set(SRCS ${SRCS} ${FORMS_HEADERS} ${RESOURCES_RCC})

add_subdirectory(deps/QtMockWebServer)

add_executable(silicatest ${SRCS})
set_target_properties(silicatest PROPERTIES MACOSX_BUNDLE FALSE)
target_link_libraries(silicatest
    quazip
    qt7z
    qtdbmigration
    qtmockwebserver
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Qt5::Test
    Qt5::Widgets
)

file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)
file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/../src/assets/migration.json
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)

# set(RUNTIME_DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# install(TARGETS silica BUNDLE DESTINATION ${RUNTIME_DESTINATION} RUNTIME DESTINATION ${RUNTIME_DESTINATION})
# 
# # Default config file
# set(SILICA_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/assets/Silica.ini)
# if (APPLE)
#     install(FILES ${SILICA_CONFIG} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/silica.app/Contents/MacOS/config/Asuna)
# else ()
#     install(FILES ${SILICA_CONFIG} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/config/Asuna)
# endif()
# 
# # Database migration config
# set(SILICA_MIGRATION_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/assets/migration.json)
# if (APPLE)
#     install(FILES ${SILICA_MIGRATION_CONFIG} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/silica.app/Contents/MacOS)
# else ()
#     install(FILES ${SILICA_MIGRATION_CONFIG} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# endif()

# install(FILES DESTINATION)
# install(TARGETS silica LIBRARY DESTINATION ${LIB_DESTINATION} ARCHIVE DESTINATION ${LIB_DESTINATION} RUNTIME DESTINATION ${LIB_DESTINATION})
