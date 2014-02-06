SOURCES += \
    $$PWD/*.cpp \
    $$PWD/db/*.cpp \
    $$PWD/ui/*.cpp


HEADERS += \
    $$PWD/*.h \
    $$PWD/db/*.h \
    $$PWD/ui/*.h

INCLUDEPATH += \
    db \
    ui

FORMS += $$PWD/*.ui

RESOURCES += $$PWD/*.qrc

INCLUDEPATH += $$PWD/deps/quazip/quazip
DEPENDPATH += $$PWD/deps/quazip/quazip
