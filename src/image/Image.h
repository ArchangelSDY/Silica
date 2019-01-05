#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QObject>
#include <QWeakPointer>
#include <QUuid>
#include <QVariant>

class QThreadPool;

class ImageHotspot;
class ImageRank;
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

    friend class ImageHotspot;
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
    qreal aspectRatio() const;
    const ImageSource *source() const { return m_imageSource.data(); }

    bool isLoading() const;
    bool isError() const;

    void load(int priority = NormalPriority, bool forceReload = false);
    void loadThumbnail();
    QSharedPointer<QImage> loadThumbnailSync();

    void loadMetadata();

    QList<ImageHotspot *> hotspots() { return m_hotspots; }
    void loadHotspots(bool forceReload = false);

    ImageRank *rank() { return m_rank; }

    bool copy(const QString &destPath);

    inline bool operator ==(const Image &other)
    {
        return m_imageSource == other.m_imageSource;
    }

    int width() const { return m_size.width(); }
    int height() const { return m_size.height(); }
    QSize size() const { return m_size; }
    QVariantHash &metadata() { return m_metadata; }

    QWeakPointer<ImageData> image() const { return m_image; }

signals:
    void loaded(QSharedPointer<ImageData> image);
    void thumbnailLoaded(QSharedPointer<QImage> thumbnail);
    void thumbnailLoadFailed();
    void hotpotsLoaded();

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
    QSize m_thumbnailSize;
    QString m_thumbnailPath;

    bool m_isLoadingImage;
    bool m_isLoadingThumbnail;
    bool m_isMakingThumbnail;
    bool m_isError;
    bool m_needMakeThumbnail;

    QList<ImageHotspot *> m_hotspots;
    bool m_hotspotsLoaded;

    ImageRank *m_rank;

    QSize m_size;
    QVariantHash m_metadata;

    QWeakPointer<ImageData> m_image;
    bool m_isAnimation;
};

typedef QList<QSharedPointer<Image> > ImageList;
typedef QSharedPointer<Image> ImagePtr;

#endif // IMAGE_H
