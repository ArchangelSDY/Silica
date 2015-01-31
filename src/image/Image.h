#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QtGui>


class QUuid;

class ImageHistogram;
class ImageHotspot;
class ImageRank;
class ImageSource;

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

    QUuid uuid() const { return m_uuid; }
    Status status() const { return m_status; }
    QImage data()
    {
        return *(defaultFrame());
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
    QVariantHash &extraInfo() { return m_extraInfo; }

    bool isAnimation() const;
    QList<int> durations() const;
    QList<QImage *> frames() const;
    int frameCount() const;

    ImageHistogram *thumbHist() const;

signals:
    void loaded();
    void thumbnailLoaded();
    void thumbnailLoadFailed();
    void hotpotsLoaded();

private slots:
    void imageReaderFinished(QList<QSharedPointer<QImage> > images, QList<int> durations);
    void thumbnailReaderFinished(QSharedPointer<QImage> thumbnail, bool makeImmediately);
    void thumbnailMade(QSharedPointer<QImage> thumbnail);
    void initThumbHist();

private:
    inline QImage *defaultFrame() const
    {
        Q_ASSERT(!m_frames.isEmpty());
        return m_frames[0];
    }

    void destroyFrames();
    void resetFrames(QImage *defaultFrame = new QImage(),
                     int defaultDuration = 0);
    void unloadIfNeeded();
    void makeThumbnail();
    void computeThumbnailPath();
    void loadMetaFromDatabase();

    QUuid m_uuid;
    Status m_status;
    QSharedPointer<ImageSource> m_imageSource;
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
    QVariantHash m_extraInfo;

    QList<QImage *> m_frames;
    QList<int> m_durations;
    bool m_isAnimation;

    ImageHistogram *m_thumbHist;
};

typedef QList<QSharedPointer<Image> > ImageList;
typedef QSharedPointer<Image> ImagePtr;

#endif // IMAGE_H
