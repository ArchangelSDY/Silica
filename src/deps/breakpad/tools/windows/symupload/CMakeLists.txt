cmake_minimum_required(VERSION 3.5)
project (silica-symupload)

if (WIN32)

if (NOT (EXISTS "${DEBUG_ACCESS_SDK}"))
    message(FATAL_ERROR "Debug Access SDK not found in ${DEBUG_ACCESS_SDK}")
endif()

add_definitions(
	-D_WIN32_WINNT=0x0600
	-DWINVER=0x0600
	-DWIN32
	-D_WINDOWS
	-D_HAS_EXCEPTIONS=0
	-DNOMINMAX
	-D_CRT_RAND_S
	-DCERT_CHAIN_PARA_HAS_EXTRA_FIELDS
	-DWIN32_LEAN_AND_MEAN
	-D_SECURE_ATL
	-D_HAS_TR1=0
	-D_USING_V110_SDK71_
	-DUNICODE
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../../../
    ${DEBUG_ACCESS_SDK}/include
)
find_library(DIA_SDK_GUID_LIB diaguids PATHS ${DEBUG_ACCESS_SDK}/lib)

set(SYMUPLOAD_SRCS
    "../../../common/windows/dia_util.cc"
    "../../../common/windows/guid_string.cc"
    "../../../common/windows/http_upload.cc"
    "../../../common/windows/omap.cc"
    "../../../common/windows/pdb_source_line_writer.cc"
    "../../../common/windows/string_utils.cc"
    "symupload.cc"
)

add_executable(symupload ${SYMUPLOAD_SRCS})
target_link_libraries(symupload ${DIA_SDK_GUID_LIB} Imagehlp Wininet Mincore)

endif(WIN32)
