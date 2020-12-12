cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0042 NEW)   # Set MACOSX_RPATH default to ON
cmake_policy(SET CMP0048 NEW)   # Set PROJECT_VERSION properly

project (silica VERSION 0.3.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Threads)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gamepad REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Pdf REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(OpenCV REQUIRED)

set(QTCORE_LIBRARIES ${Qt5Core_LIBRARIES})
# if there is no QT_ROOT, try to deduce it from Qt QtCore include
if ("${QT_ROOT}" STREQUAL "")
    set(QT_ROOT ${Qt5Core_DIR}/../../../)
endif()

if (WIN32)
    find_package(OpenSSL REQUIRED)
endif (WIN32)

if (UNIX)
    find_package(ZLIB REQUIRED)
else (UNIX)
    set(ZLIB_INCLUDE_DIRS "${QT_ROOT}/include/QtZlib" CACHE STRING "Path to ZLIB headers of Qt")
    set(ZLIB_LIBRARIES "")
    if(NOT EXISTS "${ZLIB_INCLUDE_DIRS}/zlib.h")
        message(FATAL_ERROR "Please specify a valid zlib include dir")
    endif(NOT EXISTS "${ZLIB_INCLUDE_DIRS}/zlib.h")
endif (UNIX)

include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${Qt5Concurrent_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gamepad_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Pdf_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

# All build libraries are moved to this directory
# SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
#
# set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
# set(LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)

# OpenGL support
option(ENABLE_OPENGL "Enable OpenGL" OFF)
if(ENABLE_OPENGL)
    if (NOT WIN32)
        add_definitions(-DENABLE_OPENGL)
        find_package(Qt5OpenGL REQUIRED)
        include_directories(${Qt5OpenGL_INCLUDE_DIRS})
	else()
        message(WARNING "OpenGL cannot be enabled on Windows because we use DirectX for hardware acceleration.")
        set(ENABLE_OPENGL OFF)
    endif()
endif(ENABLE_OPENGL)

# DirectX support
if (WIN32)
    find_package(WindowsSDK REQUIRED)
    get_windowssdk_include_dirs(${WINDOWSSDK_LATEST_DIR} WINDOWSSDK_INCLUDE_DIRS)
    get_windowssdk_library_dirs(${WINDOWSSDK_LATEST_DIR} WINDOWSSDK_LIB_DIRS)

    include_directories(${WINDOWSSDK_INCLUDE_DIRS})
endif()

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
set(CMAKE_AUTOMOC ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_PRODUCTION "Build for production" OFF)
message(STATUS "Build Production: ${BUILD_PRODUCTION}")
if (BUILD_PRODUCTION)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif(BUILD_PRODUCTION)

# Enable C++11 for gcc and clang
if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
	set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif ()

if (WIN32)
    # Enable multi-processor compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

    # Enable pdb generation for release build on Windows
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO")
endif (WIN32)

add_subdirectory(src)

# Disable testing in production build in order to avoid occasional clearing
# production database
if(NOT BUILD_PRODUCTION)
    enable_testing()
    add_subdirectory(test)
endif()
