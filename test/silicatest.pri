SOURCES += \
    silicatest.cpp \
    TestImageSource.cpp \
    TestPlayList.cpp

HEADERS += \
    TestImageSource.h \
    TestPlayList.h

assets.files = $$PWD/assets/*
assets.path = $$OUT_PWD/assets

INSTALLS += assets
