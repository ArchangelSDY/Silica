SOURCES += \
    silicatest.cpp \
    TestImage.cpp \
    TestImageRank.cpp \
    TestImageSource.cpp \
    TestLocalDatabase.cpp \
    TestPlayList.cpp

HEADERS += \
    TestImage.h \
    TestImageRank.h \
    TestImageSource.h \
    TestLocalDatabase.h \
    TestPlayList.h

assets.files = $$PWD/assets/*
assets.path = $$OUT_PWD/assets

INSTALLS += assets

migration.files = $$PWD/../src/assets/migration.json
migration.path = $$OUT_PWD

INSTALLS += migration
