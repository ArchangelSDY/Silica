#include "CGColleV0Reader.h"

#include <QBuffer>
#include <QImage>
#include <QPainter>

class CGColleV0ImageReader : public CGColleReader::ImageReader
{
public:
    CGColleV0ImageReader(const QString &packagePath,
                         bool isBase,
                         uint32_t dataPos,
                         uint32_t dataSize,
                         uint32_t baseDataPos,
                         uint32_t baseDataSize) :
        m_packagePath(packagePath),
        m_isBase(isBase),
        m_dataPos(dataPos),
        m_dataSize(dataSize),
        m_baseDataPos(baseDataPos),
        m_baseDataSize(baseDataSize)
    {
    }
    
    virtual QByteArray read() override;

private:
    QString m_packagePath;
    bool m_isBase;
    uint32_t m_dataPos;
    uint32_t m_dataSize;
    uint32_t m_baseDataPos;
    uint32_t m_baseDataSize;
    uint32_t m_offsetX;
    uint32_t m_offsetY;
};

QByteArray CGColleV0ImageReader::read()
{
    QFile f(m_packagePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }

    uchar *ptr = f.map(0, f.size());

    if (m_isBase) {
        // Base frame
        return QByteArray(reinterpret_cast<const char *>(ptr + m_dataPos), m_dataSize);
    } else {
        // Layer frame
        QImage frame = QImage::fromData(ptr + m_baseDataPos, m_baseDataSize);
        QImage subFrame = QImage::fromData(ptr + m_dataPos, m_dataSize);

        QPainter painter(&frame);
        painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
        painter.drawImage(QPointF(), subFrame);

        QByteArray frameData;
        QBuffer buf(&frameData);
        buf.open(QIODevice::WriteOnly);
        frame.save(&buf, "BMP");

        return frameData;
    }
}

CGColleV0Reader::CGColleV0Reader(const QString &packagePath) :
    m_file(packagePath)
{
}

CGColleV0Reader::~CGColleV0Reader()
{
    clear();
}

void CGColleV0Reader::clear()
{
    foreach (CGColleImage *image, m_images) {
        delete image;
    }
    m_images.clear();
    m_imagesByName.clear();
    m_imageNames.clear();
}

bool CGColleV0Reader::open()
{
    if (!m_file.open(QIODevice::ReadOnly)) {
        return false;
    }

    m_data = m_file.map(0, m_file.size());

    // Check format and scan meta
    if (!isValidFormat() || !scanMeta()) {
        m_file.close();
        return false;
    }

    return true;
}

bool CGColleV0Reader::isValidFormat(const char *ptr)
{
    static const char RIFF[4] = { 'R','I', 'F', 'F' };
    if (qstrncmp(ptr, RIFF, 4) != 0) {
        return false;
    }

    static const char CGC[4] = { 'C','G', 'C', 0 };
    if (qstrncmp(reinterpret_cast<const char *>(ptr + 8), CGC, 4) != 0) {
        return false;
    }

    return true;
}

bool CGColleV0Reader::isValidFormat() const
{
    return CGColleV0Reader::isValidFormat(reinterpret_cast<const char *>(m_data));
}

bool CGColleV0Reader::isValidFormat(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    uchar *ptr = f.map(0, 12);
    if (!ptr) {
        return false;
    }

    return CGColleV0Reader::isValidFormat(reinterpret_cast<const char *>(ptr));
}

bool CGColleV0Reader::scanMeta()
{
    // Find meta chunk
    static const char META[4] = { 'M', 'E', 'T', 'A' };
    uchar *metaPtr = seekChunk(META);
    if (metaPtr == nullptr) {
        return false;
    }

    // Skip meta chunk name
    metaPtr += 4;

    uint32_t metaSize = *reinterpret_cast<uint32_t*>(metaPtr);
    metaPtr += sizeof(uint32_t);
    uint32_t imagesCount = *reinterpret_cast<uint32_t*>(metaPtr);
    metaPtr += sizeof(uint32_t);

    // Find data chunk
    static const char DATA[4] = { 'D','A','T','A' };
    uchar *dataPtr = seekChunk(DATA);
    if (dataPtr == nullptr) {
        return false;
    }
    uint32_t imageOffset = dataPtr - m_data + 8;

    for (uint32_t i = 0; i < imagesCount; ++i) {
        CGColleImage *image = new CGColleImage();
        image->id = i;

        uint32_t nameSize = *reinterpret_cast<uint32_t*>(metaPtr);
        metaPtr += sizeof(nameSize);

        image->name = QString::fromUtf8(reinterpret_cast<const char *>(metaPtr), nameSize);
        metaPtr += nameSize;

        image->mainFrameId = *reinterpret_cast<uint32_t*>(metaPtr);
        metaPtr += sizeof(image->mainFrameId);

        image->size = *reinterpret_cast<uint32_t*>(metaPtr);
        metaPtr += sizeof(image->size);

        image->offset = imageOffset;
        imageOffset += image->size;

        m_images << image;
        m_imageNames << image->name;
        m_imagesByName.insert(image->name, image);
    }

    return true;
}

uchar *CGColleV0Reader::seekChunk(const char chunkName[4])
{
    uchar *ptr = m_data + 12;
    while (ptr - m_data < m_file.size()) {
        if (qstrncmp(reinterpret_cast<const char *>(ptr), chunkName, 4) == 0) {
            return ptr;
        } else {
            uint32_t chunkSize = *reinterpret_cast<uint32_t*>(ptr + 4);
            ptr += (4 + sizeof(uint32_t) + chunkSize);
        }
    }

    return nullptr;
}

void CGColleV0Reader::close()
{
    clear();
    m_file.close();
}

CGColleReader::ImageReader *CGColleV0Reader::createReader(const QString &imageName)
{
    auto it = m_imagesByName.find(imageName);
    if (it == m_imagesByName.end()) {
        return nullptr;
    }

    CGColleImage *imageMeta = *it;
    if (imageMeta->id == imageMeta->mainFrameId) {
        // Main frame
        return new CGColleV0ImageReader(
            m_file.fileName(),
            true,
            imageMeta->offset,
            imageMeta->size,
            0,
            0
        );
    } else {
        // Sub frame
        CGColleImage *mainFrameMeta = m_images[imageMeta->mainFrameId];
        return new CGColleV0ImageReader(
            m_file.fileName(),
            false,
            imageMeta->offset,
            imageMeta->size,
            mainFrameMeta->offset,
            mainFrameMeta->size
        );
    }
}

QStringList CGColleV0Reader::imageNames() const
{
    return m_imageNames;
}