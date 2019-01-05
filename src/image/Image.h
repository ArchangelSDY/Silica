#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QObject>
#include <QWeakPointer>
#include <QUuid>
#include <QVariant>

class QThreadPool;

class ImageSource;

class ImageData
{
public:
    bool isAnimation() const
    {
        return frames.count() > 1;
    }

    QImage defaultFrame() const
    {
        Q_ASSERT(frames.count() > 0);
        return frames.first();
    }

    QList<QImage> frames;
    QList<int> durations;
};

class Image : public QObject
{
    Q_OBJECT
public:
    explicit Image(const QString &path, QObject *parent = 0);
    explicit Image(const QUrl &url, QObject *parent = 0);
    explicit Image(QSharedPointer<ImageSource> imageSource, QObject *parent = 0);
    ~Image();

    enum LoadPriority {
        NormalPriority = 0,
        LowPriority = -1,
        LowestPriority = -2,
    };

    QUuid uuid() const { return m_uuid; }
    QString name() const;
    QString thumbnailPath() const { return m_thumbnailPath; }
    QSharedPointer<ImageSource> source() const { return m_imageSource; }
    QVariantHash &metadata() { return m_metadata; }
    QWeakPointer<ImageData> data() const { return m_data; }

    int width() const { return m_size.width(); }
    int height() const { return m_size.height(); }
    QSize size() const { return m_size; }
    qreal aspectRatio() const;

    bool isLoading() const;
    bool isError() const;

    void load(int priority = NormalPriority, bool forceReload = false);
    void loadThumbnail();
    QSharedPointer<QImage> loadThumbnailSync();
    void makeThumbnail();
    void loadMetadata();

signals:
    void loaded(QSharedPointer<ImageData> image);
    void thumbnailLoaded(QSharedPointer<QImage> thumbnail);
    void thumbnailLoadFailed();

private slots:
    void imageReaderFinished(QVariantHash metadata, QSharedPointer<ImageData> image);
    void thumbnailReaderFinished(QSharedPointer<QImage> thumbnail);
    void thumbnailMade(QSharedPointer<QImage> thumbnail);

    void onLoad(QSharedPointer<ImageData> image);
    void onThumbnailLoadFailed();

private:
    static QThreadPool *s_threadPool;

    QImage defaultFrame() const;
    void makeThumbnail(QSharedPointer<ImageData> image);
    void computeThumbnailPath();
    void resetMetadata(const QVariantHash &metadata);

    QUuid m_uuid;
    QSharedPointer<ImageSource> m_imageSource;
    QWeakPointer<QImage> m_thumbnail;
    QWeakPointer<ImageData> m_data;

    QSize m_size;
    QSize m_thumbnailSize;
    QString m_thumbnailPath;
    QVariantHash m_metadata;

    bool m_isLoadingImage;
    bool m_isLoadingThumbnail;
    bool m_isMakingThumbnail;
    bool m_isError;
    bool m_needMakeThumbnail;
};

typedef QList<QSharedPointer<Image> > ImageList;
typedef QSharedPointer<Image> ImagePtr;

#endif // IMAGE_H
