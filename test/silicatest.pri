SOURCES += \
    TestZipImageSource.cpp \
    silicatest.cpp

HEADERS += \
    TestZipImageSource.h

assets.files = $$PWD/assets/*

macx {
    assets.path = $$OUT_PWD/silicatest.app/Contents/MacOS/assets

    INSTALLS += assets
}
