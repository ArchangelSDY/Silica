file(GLOB SILICA_UI_SRCS
    "ui/*.cpp"
    "ui/hotspots/*.cpp"
    "ui/renderers/*.cpp"
    "MainWindow.cpp"
)

file(GLOB SILICA_UIS
    "*.ui"
    "navigation/*.ui"
    "ui/*.ui"
)
qt5_wrap_ui(SILICA_UI_HEADERS ${SILICA_UIS})

set(SILICA_UI_SRCS ${SILICA_UI_SRCS} ${SILICA_UI_HEADERS} ${DEFINITION_SRCS})

add_library(silicaui STATIC ${SILICA_UI_SRCS})
target_link_libraries(silicaui sapi silicacore)
