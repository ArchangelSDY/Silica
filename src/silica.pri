SOURCES += main.cpp\
    CommandInterpreter.cpp \
    GlobalConfig.cpp \
    Image.cpp \
    ImagesCache.cpp \
    ImageSource.cpp \
    LocalImageSource.cpp \
    MainWindow.cpp \
    Navigator.cpp \
    PlayList.cpp \
    ZipImageSource.cpp \
    db/Database.cpp \
    db/AsunaDatabase.cpp \
    ui/GalleryItem.cpp \
    ui/GalleryView.cpp \
    ui/MainGraphicsView.cpp

HEADERS  += \
    CommandInterpreter.h \
    GlobalConfig.h \
    Image.h \
    ImagesCache.h \
    ImageSource.h \
    LocalImageSource.h \
    MainWindow.h \
    Navigator.h \
    PlayList.h \
    ZipImageSource.h \
    db/Database.h \
    db/AsunaDatabase.h \
    ui/GalleryItem.h \
    ui/GalleryView.h \
    ui/MainGraphicsView.h

FORMS    += MainWindow.ui

RESOURCES += \
    silica.qrc

INCLUDEPATH += $$PWD/deps/quazip/quazip
DEPENDPATH += $$PWD/deps/quazip/quazip
