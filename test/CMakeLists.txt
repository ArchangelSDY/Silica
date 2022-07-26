add_subdirectory(deps/QtMockWebServer)
set_target_properties(qtmockwebserver PROPERTIES FOLDER "Dependencies")
set(INSTALL_GTEST OFF)
set(gtest_force_shared_crt ON)
add_subdirectory(deps/googletest)
set_target_properties(gtest gtest_main PROPERTIES FOLDER "Dependencies")
set_target_properties(gmock gmock_main PROPERTIES FOLDER "Dependencies")

# set all include directories for in and out of source builds
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/QtMockWebServer
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/googletest/googlemock/include
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

qt_wrap_ui(TEST_COMMON_FORMS_HEADERS ${TEST_COMMON_FORMS})
qt_add_resources(TEST_COMMON_RESOURCES_RCC ${TEST_COMMON_RESOURCES})
set(TEST_COMMON_SRCS ${TEST_COMMON_SRCS} ${TEST_COMMON_FORMS_HEADERS} ${TEST_COMMON_RESOURCES_RCC})


set(SILICA_TEST_COMMON_LINK_LIBRARIES
    sapi
    silicacoreobjs
    silicaui
    qtmockwebserver
    gmock_main
    Qt::Test
)
add_library(silicatestcommon ${TEST_COMMON_SRCS})
target_link_libraries(silicatestcommon ${SILICA_TEST_COMMON_LINK_LIBRARIES})
set_target_properties(silicatestcommon PROPERTIES
    FOLDER "Tests"
)

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
    set_target_properties(${TEST_TARGET} PROPERTIES
        MACOSX_BUNDLE FALSE
        FOLDER "Tests"
    )
    target_link_libraries(${TEST_TARGET} silicatestcommon)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_TARGET} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    if(WIN32)
        set_property(TEST ${TEST_NAME} PROPERTY ENVIRONMENT "PATH=%PATH%\;$<TARGET_FILE_DIR:Qt6::Core>\;$<TARGET_FILE_DIR:opencv_core>\;$<TARGET_FILE_DIR:ZLIB::ZLIB>/../bin")
    endif(WIN32)
endforeach(TEST_FILE)
