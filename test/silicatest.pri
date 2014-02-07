SOURCES += \
    silicatest.cpp \
    TestImageSource.cpp \
    TestPlayList.cpp

HEADERS += \
    TestImageSource.h \
    TestPlayList.h

assets.files = $$PWD/assets/*

macx {
    assets.path = $$OUT_PWD/silicatest.app/Contents/MacOS/assets

    INSTALLS += assets
}
