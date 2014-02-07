#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QtGui>

#include "ImageSource.h"

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

class Image : public QObject
{
    Q_OBJECT
public:
    explicit Image(QUrl url, QObject *parent = 0);
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
    qreal aspectRatio() const;

    void load(int priority = NormalPriority);
    void loadThumbnail(bool makeImmediately = false);
    void scheduleUnload();

    bool copy(const QString &destPath);

    inline bool operator ==(const Image &other)
    {
        return m_imageSource == other.m_imageSource;
    }

signals:
    void loaded();
    void thumbnailLoaded();

public slots:
    void imageReaderFinished(QImage *image);
    void thumbnailReaderFinished(QImage *thumbnail, bool makeImmediately);

private:
    void unloadIfNeeded();
    void makeThumbnail();
    void computeThumbnailPath();

    Status m_status;
    QSharedPointer<ImageSource> m_imageSource;
    QImage *m_image;
    QImage *m_thumbnail;
    QString m_thumbnailPath;
    int m_loadRequestsCount;

    bool m_isLoadingImage;
    bool m_isLoadingThumbnail;
};

#endif // IMAGE_H
