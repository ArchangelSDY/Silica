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

file(GLOB EXTERNAL_SRCS
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
    "utils/*.cpp"
)

file(GLOB FORMS "../src/*.ui" "../src/ui/*.ui")
file(GLOB RESOURCES "../src/*.qrc")

qt5_wrap_ui(FORMS_HEADERS ${FORMS})
qt5_add_resources(RESOURCES_RCC ${RESOURCES})
set(EXTERNAL_SRCS ${EXTERNAL_SRCS} ${FORMS_HEADERS} ${RESOURCES_RCC})

add_subdirectory(deps/QtMockWebServer)

file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)
file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/../src/assets/migration.json
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)

set(TEST_LINK_LIBRARIES
    qt7z
    qtdbmigration
    qtmockwebserver
    qtrar
    quazip
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Qt5::Test
    Qt5::Widgets
)

set(TEST_ROOT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
string(LENGTH ${TEST_ROOT_DIRECTORY} TEST_ROOT_DIRECTORY_LENGTH)
math(EXPR TEST_ROOT_DIRECTORY_LENGTH "${TEST_ROOT_DIRECTORY_LENGTH} + 1")

file(GLOB TEST_FILES *_Tests.cpp)
foreach(TEST_FILE ${TEST_FILES})
    string(SUBSTRING ${TEST_FILE} ${TEST_ROOT_DIRECTORY_LENGTH} -1 TEST_FILE_RELATIVE)
    string(LENGTH ${TEST_FILE_RELATIVE} TEST_FILE_RELATIVE_LENGTH)
    math(EXPR TEST_NAME_LENGTH "${TEST_FILE_RELATIVE_LENGTH} - 4")
    string(SUBSTRING ${TEST_FILE_RELATIVE} 0 ${TEST_NAME_LENGTH} TEST_NAME)

    get_filename_component(TEST_TARGET ${TEST_FILE} NAME_WE)

    add_executable(${TEST_TARGET} ${TEST_FILE} ${EXTERNAL_SRCS})
    set_target_properties(${TEST_TARGET} PROPERTIES MACOSX_BUNDLE FALSE)
    target_link_libraries(${TEST_TARGET} ${TEST_LINK_LIBRARIES})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_TARGET} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endforeach(TEST_FILE)
