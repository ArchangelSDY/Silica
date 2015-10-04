set(SILICA_SRCS
    "main.cpp"
)
file(GLOB SILICA_RESOURCES "*.qrc")
qt5_add_resources(SILICA_RESOURCES_RCC ${SILICA_RESOURCES})
set(SILICA_SRCS ${SILICA_SRCS} ${SILICA_RESOURCES_RCC} ${DEFINITION_SRCS})

if (WIN32)
    add_executable(silica WIN32 ${SILICA_SRCS})
else (WIN32)
    add_executable(silica ${SILICA_SRCS})
endif (WIN32)

target_link_libraries(silica sapi silicacore)
set_target_properties(silica PROPERTIES INSTALL_RPATH "@executable_path;@executable_path/../Resources/lib;$ORIGIN/../lib")

# WebP Animation Plugin
add_custom_command(TARGET silica POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_BINARY_DIR}/deps/qwebpa/src/imageformats
    $<TARGET_FILE_DIR:silica>/plugins/imageformats
)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins" DESTINATION "lib" COMPONENT applications)

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

else ()

    install(TARGETS silica
        RUNTIME DESTINATION bin
        COMPONENT applications
    )

endif()
