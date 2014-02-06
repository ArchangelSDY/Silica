SOURCES += \
    silicatest.cpp \
    TestImageSource.cpp

HEADERS += \
    TestImageSource.h

assets.files = $$PWD/assets/*

macx {
    assets.path = $$OUT_PWD/silicatest.app/Contents/MacOS/assets

    INSTALLS += assets
}
