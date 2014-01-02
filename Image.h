#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QtGui>

class Image : public QObject
{
    Q_OBJECT
public:
    explicit Image(QUrl url, QObject *parent = 0);

    enum Status {
        NotLoad,
        Loading,
        LoadComplete,
    };

    Status status() const { return m_status; }
    QImage data() { return m_image; }
    QImage thumbnail() { return m_thumbnail; }
    QUrl url() const { return m_url; }
    QString name() const;

    void load();
    void loadThumbnail();

signals:
    void loaded();
    void thumbnailLoaded();

public slots:
    void readerFinished();

private:
    void makeThumbnail();
    void computeThumbnailPath();

    Status m_status;
    QUrl m_url;
    QImage m_image;
    QImage m_thumbnail;
    QString m_thumbnailPath;

    QFuture<QImage> m_readerFuture;
    QFutureWatcher<QImage> m_readerWatcher;
};

#endif // IMAGE_H
