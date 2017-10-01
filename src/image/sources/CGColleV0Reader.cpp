#include "CGColleV0Reader.h"

#include <QBuffer>
#include <QImage>
#include <QPainter>

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

QByteArray CGColleV0Reader::read(const QString &imageName)
{
    auto it = m_imagesByName.find(imageName);
    if (it == m_imagesByName.end()) {
        return QByteArray();
    }

    CGColleImage *imageMeta = *it;
    if (imageMeta->id == imageMeta->mainFrameId) {
        // Main frame
        return QByteArray(reinterpret_cast<const char *>(m_data + imageMeta->offset), imageMeta->size);
    } else {
        // Sub frame
        CGColleImage *mainFrameMeta = m_images[imageMeta->mainFrameId];

        QImage frame = QImage::fromData(m_data + mainFrameMeta->offset, mainFrameMeta->size);
        QImage subFrame = QImage::fromData(m_data + imageMeta->offset, imageMeta->size);

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

QStringList CGColleV0Reader::imageNames() const
{
    return m_imageNames;
}