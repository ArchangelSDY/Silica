#pragma once

#include <QFile>
#include <QHash>
#include <QList>

#include "CGColleReader.h"

class CGColleV1Reader : public CGColleReader
{
public:
    class CGColleV1Image;

    CGColleV1Reader(const QString &packagePath);
    virtual ~CGColleV1Reader() override;

    virtual bool open() override;
    virtual void close() override;

    virtual CGColleReader::ImageReader *createReader(const QString &imageName) override;
    virtual QStringList imageNames() const override;

    static bool isValidFormat(const QString &path);

private:
    static bool isValidFormat(const char *ptr);
    bool isValidFormat() const;
    bool scanMeta();
    void clear();

    QFile m_file;
    uint32_t m_dataStart;
    QStringList m_imageNames;
    QHash<QString, CGColleV1Image *> m_imagesByName;
    QList<CGColleV1Image *> m_images;
};
