add_subdirectory(deps/QtMockWebServer)

# set all include directories for in and out of source builds
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/QtMockWebServer
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(CMAKE_AUTOMOC ON)

set(BUILD_ENV "Silica-Test")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/Definitions.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Definitions.cpp"
    @ONLY
)

file(GLOB TEST_COMMON_SRCS
    "utils/*.cpp"
    "STestCase.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/Definitions.cpp"
)

file(GLOB TEST_COMMON_RESOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.qrc"
    "*.qrc"
)

qt5_wrap_ui(TEST_COMMON_FORMS_HEADERS ${TEST_COMMON_FORMS})
qt5_add_resources(TEST_COMMON_RESOURCES_RCC ${TEST_COMMON_RESOURCES})
set(TEST_COMMON_SRCS ${TEST_COMMON_SRCS} ${TEST_COMMON_FORMS_HEADERS} ${TEST_COMMON_RESOURCES_RCC})


set(SILICA_TEST_COMMON_LINK_LIBRARIES
    sapi
    silicacoreobjs
    silicaui
    qtmockwebserver
    Qt5::Test
)
add_library(silicatestcommon ${TEST_COMMON_SRCS})
target_link_libraries(silicatestcommon ${SILICA_TEST_COMMON_LINK_LIBRARIES})

# Copy test assets
add_custom_command(TARGET silicatestcommon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:silicatestcommon>/assets
)

if (WIN32)
    add_custom_command(TARGET silicatestcommon POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:sapi>
        $<TARGET_FILE_DIR:silicatestcommon>/
    )
endif()

set(TEST_ROOT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
string(LENGTH ${TEST_ROOT_DIRECTORY} TEST_ROOT_DIRECTORY_LENGTH)
math(EXPR TEST_ROOT_DIRECTORY_LENGTH "${TEST_ROOT_DIRECTORY_LENGTH} + 1")

file(GLOB_RECURSE TEST_FILES *_Tests.cpp)
foreach(TEST_FILE ${TEST_FILES})
    string(SUBSTRING ${TEST_FILE} ${TEST_ROOT_DIRECTORY_LENGTH} -1 TEST_FILE_RELATIVE)
    string(LENGTH ${TEST_FILE_RELATIVE} TEST_FILE_RELATIVE_LENGTH)
    math(EXPR TEST_NAME_LENGTH "${TEST_FILE_RELATIVE_LENGTH} - 4")
    string(SUBSTRING ${TEST_FILE_RELATIVE} 0 ${TEST_NAME_LENGTH} TEST_NAME)

    get_filename_component(TEST_TARGET ${TEST_FILE} NAME_WE)

    add_executable(${TEST_TARGET} ${TEST_FILE})
    set_target_properties(${TEST_TARGET} PROPERTIES MACOSX_BUNDLE FALSE)
    target_link_libraries(${TEST_TARGET} silicatestcommon)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_TARGET} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    if(WIN32)
        set_property(TEST ${TEST_NAME} PROPERTY ENVIRONMENT "PATH=%PATH%\;${QT_ROOT}bin;${ZLIB_INCLUDE_DIR}/../bin;${OPENSSL_INCLUDE_DIR}/../lib")
    endif(WIN32)
endforeach(TEST_FILE)
