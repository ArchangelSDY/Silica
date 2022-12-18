cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0042 NEW)   # Set MACOSX_RPATH default to ON
cmake_policy(SET CMP0048 NEW)   # Set PROJECT_VERSION properly

project (silica VERSION 3.2.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(ZLIB REQUIRED)
find_package(Threads)
find_package(Qt6Concurrent REQUIRED)
find_package(Qt6Core REQUIRED)
find_package(Qt6Network REQUIRED)
find_package(Qt6Pdf REQUIRED)
find_package(Qt6Sql REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Test REQUIRED)
find_package(OpenCV REQUIRED)

set(QTCORE_LIBRARIES ${Qt6Core_LIBRARIES})
# if there is no QT_ROOT, try to deduce it from Qt QtCore include
if ("${QT_ROOT}" STREQUAL "")
    set(QT_ROOT ${Qt6Core_DIR}/../../../)
endif()

# All build libraries are moved to this directory
# SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
#
# set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
# set(LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)

# OpenGL support
option(ENABLE_OPENGL "Enable OpenGL" OFF)
if(ENABLE_OPENGL)
	add_definitions(-DENABLE_OPENGL)
	find_package(Qt6OpenGL REQUIRED)
	find_package(Qt6OpenGLWidgets REQUIRED)
endif(ENABLE_OPENGL)

# DirectX support
if (WIN32)
    find_package(WindowsSDK REQUIRED)
    get_windowssdk_include_dirs(${WINDOWSSDK_LATEST_DIR} WINDOWSSDK_INCLUDE_DIRS)
    get_windowssdk_library_dirs(${WINDOWSSDK_LATEST_DIR} WINDOWSSDK_LIB_DIRS)

    if ("${WINDOWSSDK_INCLUDE_DIRS}" STREQUAL "NOTFOUND")
        message(FATAL_ERROR "Windows SDK not found")
    endif()
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

# Enable C++17
set (CMAKE_CXX_STANDARD 17)

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
