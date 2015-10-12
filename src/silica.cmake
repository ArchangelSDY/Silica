file(GLOB SILICA_RESOURCES "*.qrc")
qt5_add_resources(SILICA_RESOURCES_RCC ${SILICA_RESOURCES})

set(SILICA_SRCS
    ${SILICA_RESOURCES_RCC}
    "main.cpp"
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

    add_executable(silica WIN32 ${SILICA_SRCS} ${WINDOWS_RES_FILE})
else (WIN32)
    add_executable(silica ${SILICA_SRCS})
endif (WIN32)

target_link_libraries(silica sapi silicacore silicaui)
set_target_properties(silica PROPERTIES INSTALL_RPATH "@executable_path;@executable_path/../Resources/lib;$ORIGIN/../lib")

# WebP Animation Plugin
add_custom_command(TARGET silica POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:qwebpa>
    $<TARGET_FILE_DIR:silica>/plugins/imageformats/
)
add_custom_command(TARGET silica POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_CURRENT_BINARY_DIR}/plugins/imageformats/
)
add_custom_command(TARGET silica POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:qwebpa>
    ${CMAKE_CURRENT_BINARY_DIR}/plugins/imageformats/
)

if (WIN32)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins" DESTINATION . COMPONENT applications)
else()
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins" DESTINATION "lib" COMPONENT applications)
endif()

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

    install(FILES "${ZLIB_INCLUDE_DIR}/../bin/zlib.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Concurrent.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Core.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Gui.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Network.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Sql.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/Qt5Widgets.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/libEGL.dll" DESTINATION . COMPONENT applications)
    install(FILES "${QT_ROOT}/bin/libGLESv2.dll" DESTINATION . COMPONENT applications)
    install(FILES "${OpenCV_LIB_DIR_OPT}/../bin/opencv_core300.dll" DESTINATION . COMPONENT applications)
    install(FILES "${OpenCV_LIB_DIR_OPT}/../bin/opencv_imgproc300.dll" DESTINATION . COMPONENT applications)

    if (ENABLE_OPENGL)
        install(FILES "${QT_ROOT}/bin/Qt5OpenGL.dll" DESTINATION . COMPONENT applications)
    endif()

else ()

    install(TARGETS silica
        RUNTIME DESTINATION bin
        COMPONENT applications
    )

endif()