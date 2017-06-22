include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

if (WIN32)

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

set(BREAKPAD_SRCS
    "common/windows/guid_string.cc"
	# "common/windows/string_utils.cc"

	"client/windows/crash_generation/crash_generation_client.cc"
	"client/windows/crash_generation/crash_generation_server.cc"
	"client/windows/crash_generation/minidump_generator.cc"
	"client/windows/crash_generation/client_info.cc"
	"client/windows/handler/exception_handler.cc"
)

add_library(breakpad STATIC ${BREAKPAD_SRCS})

endif()
