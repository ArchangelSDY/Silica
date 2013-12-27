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
    QUrl url() const { return m_url; }

    void load();

signals:
    void loaded();

public slots:
    void readerFinished();

private:
    Status m_status;
    QUrl m_url;
    QImage m_image;

    QFuture<QImage> m_readerFuture;
    QFutureWatcher<QImage> m_readerWatcher;
    QImageReader m_reader;
};

#endif // IMAGE_H
