#ifndef IMAGESOURCEFACTORY_H
#define IMAGESOURCEFACTORY_H

#include <QObject>

class ImageSource;
class ImageSourceManager;

class ImageSourceFactory : public QObject
{
    friend class ImageSource;

    Q_OBJECT
public:
    explicit ImageSourceFactory(ImageSourceManager *m_mgr, QObject *parent = 0);

    virtual QString name() const = 0;
    virtual QStringList fileNameSuffixes() const = 0;
    virtual QString urlScheme() const = 0;
    virtual ImageSource *createSingle(const QUrl &url) = 0;
    virtual ImageSource *createSingle(const QString &path) = 0;
    virtual QList<ImageSource *> createMultiple(const QUrl &url) = 0;
    virtual QList<ImageSource *> createMultiple(const QString &path) = 0;

    virtual void clearCache() {}

protected:
    QString findRealPath(QString path);
    bool requestPassword(const QString &archivePath, QString &password);
    void passwordAccepted(const QString &archivePath, const QString &password);
    void passwordRejected(const QString &archivePath);
    bool isValidFileName(const QString &name);

    ImageSourceManager *m_mgr;
};

#endif // IMAGESOURCEFACTORY_H
