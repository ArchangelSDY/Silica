#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QtGui>

#include "ImageHotspot.h"
#include "ImageSource.h"

typedef QList<QSharedPointer<Image> > ImageList;
typedef QSharedPointer<Image> ImagePtr;

class ImageRank;

class LoadImageTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadImageTask(QSharedPointer<ImageSource> imageSource) :
        QRunnable() ,
        m_imageSource(imageSource) {}

    void run();

signals:
    void loaded(QImage *image);

private:
    QSharedPointer<ImageSource> m_imageSource;
};

class LoadThumbnailTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadThumbnailTask(QString thumbnailPath, bool makeImmediately) :
        QRunnable() ,
        m_thumbnailPath(thumbnailPath) ,
        m_makeImmediately(makeImmediately) {}

    void run();

signals:
    void loaded(QImage *thumbnail, bool makeImmediately);

private:
    QString m_thumbnailPath;
    bool m_makeImmediately;
};

class MakeThumbnailTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    MakeThumbnailTask(QImage *image, const QString &path) :
        QRunnable() ,
        m_image(image) ,
        m_path(path) {}

    ~MakeThumbnailTask()
    {
        delete m_image;
    }

    void run();

signals:
    void thumbnailMade(QImage *thumbnail);

private:
      QImage *m_image;
      QString m_path;
};


class Image : public QObject
{
    Q_OBJECT

    friend class ImageHotspot;
public:
    explicit Image(QUrl url, QObject *parent = 0);
    explicit Image(ImageSource *, QObject *parent = 0);
    ~Image();

    enum Status {
        NotLoad,
        Loading,
        LoadComplete,
        LoadError,
    };

    enum LoadPriority {
        NormalPriority = 0,
        LowPriority = -1,
        LowestPriority = -2,
    };

    Status status() const { return m_status; }
    QImage data()
    {
        Q_ASSERT(m_image);
        return *m_image;
    }
    QImage thumbnail() {
        Q_ASSERT(m_thumbnail);
        return *m_thumbnail;
    }
    QString name() const;
    QString thumbnailPath() const { return m_thumbnailPath; }
    qreal aspectRatio() const;
    const ImageSource *source() const { return m_imageSource.data(); }

    void load(int priority = NormalPriority);
    void loadThumbnail(bool makeImmediately = false);
    void scheduleUnload();

    QList<ImageHotspot *> hotspots() { return m_hotspots; }
    void loadHotspots(bool forceReload = false);

    ImageRank *rank() { return m_rank; }

    bool copy(const QString &destPath);

    inline bool operator ==(const Image &other)
    {
        return m_imageSource == other.m_imageSource;
    }

    static const QSize UNKNOWN_SIZE;

    int width() const { return m_size.width(); }
    int height() const { return m_size.height(); }
    QSize size() const { return m_size; }

signals:
    void loaded();
    void thumbnailLoaded();

    void hotpotsLoaded();

public slots:
    void imageReaderFinished(QImage *image);
    void thumbnailReaderFinished(QImage *thumbnail, bool makeImmediately);
    void thumbnailMade(QImage *thumbnail);

private:
    void unloadIfNeeded();
    void makeThumbnail();
    void computeThumbnailPath();
    void loadMetaFromDatabase();

    Status m_status;
    QSharedPointer<ImageSource> m_imageSource;
    QImage *m_image;
    QImage *m_thumbnail;
    QString m_thumbnailPath;
    int m_loadRequestsCount;

    bool m_isLoadingImage;
    bool m_isLoadingThumbnail;
    bool m_isMakingThumbnail;

    QList<ImageHotspot *> m_hotspots;
    bool m_hotspotsLoaded;

    ImageRank *m_rank;

    QSize m_size;
};

#endif // IMAGE_H
