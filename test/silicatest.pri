SOURCES += \
    silicatest.cpp \
    TestImageSource.cpp \
    TestLocalDatabase.cpp \
    TestPlayList.cpp

HEADERS += \
    TestImageSource.h \
    TestLocalDatabase.h \
    TestPlayList.h

assets.files = $$PWD/assets/*
assets.path = $$OUT_PWD/assets

INSTALLS += assets
