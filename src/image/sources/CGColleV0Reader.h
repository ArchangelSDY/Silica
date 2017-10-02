#pragma once

#include <QByteArray>
#include <QFile>
#include <QHash>
#include <QStringList>

#include "CGColleReader.h"

class CGColleV0Reader : public CGColleReader
{
public:
    class CGColleImage
    {
    public:
        QString name;
        uint32_t id;
        uint32_t mainFrameId;
        uint32_t size;
        uint32_t offset;
    };

    CGColleV0Reader(const QString &packagePath);
    virtual ~CGColleV0Reader() override;

    virtual bool open() override;
    virtual void close() override;

    virtual CGColleReader::ImageReader *createReader(const QString &imageName) override;
    virtual QStringList imageNames() const override;

    static bool isValidFormat(const QString &path);

private:
    static bool isValidFormat(const char *ptr);
    bool isValidFormat() const;
    bool scanMeta();
    uchar *seekChunk(const char chunkName[4]);
    void clear();

    QFile m_file;
    uchar *m_data;
    QStringList m_imageNames;
    QHash<QString, CGColleImage *> m_imagesByName;
    QList<CGColleImage *> m_images;
};
