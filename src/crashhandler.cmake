# Silica crash handler

if (WIN32)

    set(CRASHHANDER_SRCS
        "crashhandler/CrashHandler.cpp"
        "crashhandler/CrashHandlerDialog.cpp"
    )

    qt_wrap_ui(CRASHHANDER_UI_HEADERS
        "crashhandler/CrashHandlerDialog.ui"
    )

    add_executable(crashhandler WIN32
        ${CRASHHANDER_SRCS}
        ${CRASHHANDER_UI_HEADERS}
    )

    target_link_libraries(crashhandler
        breakpad
        Qt::Core
        Qt::Gui
        Qt::Network
        Qt::Widgets
    )

    install(TARGETS crashhandler RUNTIME DESTINATION . COMPONENT applications)

endif(WIN32)

