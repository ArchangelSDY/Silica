#ifndef IMAGESOURCEMANAGERCLIENTIMPL_H
#define IMAGESOURCEMANAGERCLIENTIMPL_H

#include "image/ImageSourceManager.h"

class KeyChain;

class ImageSourceManagerClientImpl : public ImageSourceManagerClient
{
public:
    ImageSourceManagerClientImpl(QWidget *parent);

    virtual bool requestPassword(const QString &archivePath, QString &password) override;
    virtual void passwordAccepted(const QString &archivePath, const QString &password) override;

private:
    QString credentialKey(const QString &raw);

    QWidget *m_parent;
    QScopedPointer<KeyChain> m_keyChain;
};

#endif // IMAGESOURCEMANAGERCLIENTIMPL_H
