file(GLOB SILICA_UI_SRCS
    "ui/BasketView.cpp"
    "ui/FileSystemItem.cpp"
    "ui/FileSystemView.cpp"
    "ui/GalleryItem.cpp"
    "ui/GalleryView.cpp"
    "ui/ImageGalleryItem.cpp"
    "ui/ImageGalleryView.cpp"
    "ui/ImagePathCorrectorClientImpl.cpp"
    "ui/ImagePathCorrectorDialog.cpp"
    "ui/ImageSourceManagerClientImpl.cpp"
    "ui/LoadingIndicator.cpp"
    "ui/MainGraphicsView.cpp"
    "ui/MainImageGalleryView.cpp"
    "ui/MainImageGalleryView.cpp"
    "ui/MainMenuBarManager.cpp"
    "ui/NotificationWidget.cpp"
    "ui/PlayListGalleryItem.cpp"
    "ui/PlayListGalleryView.cpp"
    "ui/PluginLogsDialog.cpp"
    "ui/RankFilterMenuManager.cpp"
    "ui/RankVoteView.cpp"
    "ui/RemoteWallpapersManager.cpp"
    "ui/ViewState.cpp"
    "ui/ViewStateManager.cpp"
    "ui/hotspots/*.cpp"
    "ui/gamepad/GamepadAxisScroller.cpp"
    "ui/gamepad/GamepadController.cpp"
    "ui/renderers/*.cpp"
    "MainWindow.cpp"
)

if (WIN32)
    set(SILICA_UI_SRCS ${SILICA_UI_SRCS}
        "ui/platform/win/D2DMainGraphicsWidget.cpp"
        "ui/platform/win/DeviceResources.cpp"
    )
endif()

file(GLOB SILICA_UIS
    "*.ui"
    "ui/*.ui"
)
qt5_wrap_ui(SILICA_UI_HEADERS ${SILICA_UIS})

set(SILICA_UI_SRCS ${SILICA_UI_SRCS} ${SILICA_UI_HEADERS} ${DEFINITION_SRCS})
set(SILICA_UI_LIBS
    sapi
    silicacoreobjs
    Qt5::Gamepad
)

if (WIN32)
    find_library(D2D1_LIB "d2d1" ${WINDOWSSDK_LIB_DIRS})
    find_library(D3D11_LIB "d3d11" ${WINDOWSSDK_LIB_DIRS})
    find_library(DXGUID_LIB "dxguid" ${WINDOWSSDK_LIB_DIRS})
    find_library(DXGI_LIB "dxgi" ${WINDOWSSDK_LIB_DIRS})
    find_library(DWRITE_LIB "DWrite" ${WINDOWSSDK_LIB_DIRS})


    set(SILICA_UI_LIBS ${SILICA_UI_LIBS}
        ${D2D1_LIB}
        ${D3D11_LIB}
        ${DXGUID_LIB}
        ${DXGI_LIB}
        ${DWRITE_LIB}
    )
endif()

add_library(silicaui STATIC ${SILICA_UI_SRCS})
target_link_libraries(silicaui ${SILICA_UI_LIBS})
