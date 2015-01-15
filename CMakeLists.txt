project (silica)
cmake_minimum_required(VERSION 2.8.11)

find_package(Threads)
find_package(ZLIB REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(OpenCV REQUIRED)

set(QTCORE_LIBRARIES ${Qt5Core_LIBRARIES})
# if there is no QT_ROOT, try to deduce it from Qt QtCore include
if ("${QT_ROOT}" STREQUAL "")
    set(QT_ROOT ${QT_QTCORE_INCLUDE_DIR}/../..)
endif()

include_directories(${ZLIB_INCLUDE_DIRS})
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

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

if (("$CMAKE_CXX_COMPILER_ID" STREQUAL "GNU") OR ("$CMAKE_CXX_COMPILER_ID" MATCHES "Clang"))
	set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif ()

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
