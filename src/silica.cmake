file(GLOB SILICA_RESOURCES "*.qrc")
qt5_add_resources(SILICA_RESOURCES_RCC ${SILICA_RESOURCES})

set(SILICA_SRCS
    ${SILICA_RESOURCES_RCC}
    "main.cpp"
)

set(SILICA_LINK_LIBS
    sapi
    silicacore
    silicaui
)

if (WIN32)
    # Windows application icon
    set(WINDOWS_RES_SRC ${CMAKE_CURRENT_SOURCE_DIR}/assets/winresources.rc)
    set(WINDOWS_RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/winresources.obj)
    if (MSVC)
        add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
          COMMAND rc.exe /fo ${WINDOWS_RES_FILE} ${WINDOWS_RES_SRC}
        )
    else()
        add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
          COMMAND windres.exe ${WINDOWS_RES_SRC} ${WINDOWS_RES_FILE}
        )
    endif()

    # Enable breakpad for Windows
    set(SILICA_LINK_LIBS ${SILICA_LINK_LIBS} breakpad)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/breakpad)

    add_executable(silica WIN32 ${SILICA_SRCS} ${WINDOWS_RES_FILE})

else (WIN32)
    add_executable(silica ${SILICA_SRCS})
endif (WIN32)

target_link_libraries(silica ${SILICA_LINK_LIBS})
set_target_properties(silica PROPERTIES INSTALL_RPATH "@executable_path;@executable_path/../Resources/lib;$ORIGIN/../lib")

if (APPLE)

    # We have to use our own Info.plist template to support Retina display.
    set_target_properties(silica
        PROPERTIES MACOSX_BUNDLE_INFO_PLIST
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/MacOSXBundleInfo.plist.in
    )

    # Do not install executable when building for production, or executable
    # will be packed into `Resources` directory too.
    if (NOT BUILD_PRODUCTION)
        install(TARGETS silica
            BUNDLE DESTINATION .
            RUNTIME DESTINATION .
            COMPONENT applications
        )
    endif()

elseif (WIN32)

    install(TARGETS silica RUNTIME DESTINATION . COMPONENT applications)

    install(FILES "${OPENSSL_INCLUDE_DIR}/../bin/libeay32.dll" DESTINATION . COMPONENT applications)
    install(FILES "${OPENSSL_INCLUDE_DIR}/../bin/ssleay32.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Concurrent.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Core.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Gamepad.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Gui.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Network.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Sql.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Widgets.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/libEGL.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/libGLESv2.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/plugins/platforms/qwindows.dll" DESTINATION "platforms" COMPONENT applications)
    install(FILES "${QT_ROOT}/plugins/imageformats/qgif.dll" DESTINATION "plugins/imageformats" COMPONENT applications)
    install(FILES "${QT_ROOT}/plugins/imageformats/qicns.dll" DESTINATION "plugins/imageformats" COMPONENT applications)
    install(FILES "${QT_ROOT}/plugins/imageformats/qico.dll" DESTINATION "plugins/imageformats" COMPONENT applications)
    install(FILES "${QT_ROOT}/plugins/imageformats/qjpeg.dll" DESTINATION "plugins/imageformats" COMPONENT applications)
    install(FILES "${QT_ROOT}/plugins/imageformats/qsvg.dll" DESTINATION "plugins/imageformats" COMPONENT applications)
    install(FILES "${QT_ROOT}/plugins/imageformats/qtiff.dll" DESTINATION "plugins/imageformats" COMPONENT applications)
    install(FILES "${QT_ROOT}/plugins/imageformats/qwebp.dll" DESTINATION "plugins/imageformats" COMPONENT applications)
    install(FILES "${QT_ROOT}/plugins/sqldrivers/qsqlite.dll" DESTINATION "plugins/sqldrivers" COMPONENT applications)

    if (ENABLE_OPENGL)
        install(FILES "${QT_ROOT}/bin/Qt5OpenGL.dll" DESTINATION . COMPONENT applications)
    endif()

else ()

    install(TARGETS silica
        RUNTIME DESTINATION bin
        COMPONENT applications
    )

endif()
