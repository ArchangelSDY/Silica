#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QtGui>

#include "ImageSource.h"

struct LoadThumbnailResult {
    QImage *thumbnail;
    bool makeImmediately;
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

    void load();
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
    void readerFinished();
    void thumbnailReaderFinished();

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

    QFuture<QImage *> m_readerFuture;
    QFutureWatcher<QImage *> m_readerWatcher;
    QFuture<LoadThumbnailResult> m_thumbnailFuture;
    QFutureWatcher<LoadThumbnailResult> m_thumbnailWatcher;
};

#endif // IMAGE_H
