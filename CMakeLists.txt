cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0042 NEW)   # Set MACOSX_RPATH default to ON
cmake_policy(SET CMP0048 NEW)   # Set PROJECT_VERSION properly

project (silica VERSION 0.2.0)

find_package(Threads)
find_package(ZLIB REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(OpenCV REQUIRED)

set(QTCORE_LIBRARIES ${Qt5Core_LIBRARIES})
# if there is no QT_ROOT, try to deduce it from Qt QtCore include
if ("${QT_ROOT}" STREQUAL "")
    set(QT_ROOT ${Qt5Core_DIR}/../../../)
endif()

include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${Qt5Concurrent_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
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
    add_definitions(-DENABLE_OPENGL)
    find_package(Qt5OpenGL REQUIRED)
    include_directories(${Qt5OpenGL_INCLUDE_DIRS})
endif(ENABLE_OPENGL)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
set(CMAKE_AUTOMOC ON)

option(BUILD_PRODUCTION "Build for production" OFF)
message(STATUS "Build Production: ${BUILD_PRODUCTION}")
if (BUILD_PRODUCTION)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif(BUILD_PRODUCTION)

if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
	set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif ()

add_subdirectory(src)

# Disable testing in production build in order to avoid occasional clearing
# production database
if(NOT BUILD_PRODUCTION)
    enable_testing()
    add_subdirectory(test)
endif()
