#ifndef CGCOLLEREADER_H
#define CGCOLLEREADER_H

#include <QByteArray>
#include <QFile>
#include <QHash>
#include <QStringList>

class CGColleReader
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

    CGColleReader(const QString &packagePath);
    ~CGColleReader();

    bool open();
    void close();

    QByteArray read(const QString &imageName);
    QByteArray read(uint32_t id);
    QStringList imageNames() const;

private:
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

#endif // CGCOLLEREADER_H