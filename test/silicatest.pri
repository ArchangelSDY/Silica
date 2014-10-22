SOURCES += \
    $$PWD/silicatest.cpp \
    $$PWD/TestAsunaDatabase.cpp \
    $$PWD/TestImage.cpp \
    $$PWD/TestImageRank.cpp \
    $$PWD/TestImageSource.cpp \
    $$PWD/TestLocalDatabase.cpp \
    $$PWD/TestMultiPageReplyIterator.cpp \
    $$PWD/TestPlayList.cpp

HEADERS += \
    $$PWD/TestAsunaDatabase.h \
    $$PWD/TestImage.h \
    $$PWD/TestImageRank.h \
    $$PWD/TestImageSource.h \
    $$PWD/TestLocalDatabase.h \
    $$PWD/TestMultiPageReplyIterator.h \
    $$PWD/TestPlayList.h

assets.files = $$PWD/assets/*
assets.path = $$OUT_PWD/assets

INSTALLS += assets

migration.files = $$PWD/../src/assets/migration.json
migration.path = $$OUT_PWD

INSTALLS += migration
