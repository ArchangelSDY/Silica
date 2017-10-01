#include "CGColleV1Reader.h"

#include <QBuffer>
#include <QDataStream>
#include <QImage>
#include <QPainter>

class CGColleV1Reader::CGColleV1Image
{
public:
    enum class Type
    {
        BaseImage = 0,
        LayerImage = 1,
    };

    enum class Flag
    {
        NoFlag = 0x0,
        Hide = 0x1,
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    Type type;
    int baseImageIdx;
    int dataOffset;
    QString name;
    uint32_t fileSize;
    uint32_t width;
    uint32_t height;
    uint32_t offsetX;
    uint32_t offsetY;
    uchar layerId;
    uchar compositionMethod;
    Flags flags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CGColleV1Reader::CGColleV1Image::Flags)

CGColleV1Reader::CGColleV1Reader(const QString &packagePath) :
    m_file(packagePath)
{
}

CGColleV1Reader::~CGColleV1Reader()
{
    clear();
}

void CGColleV1Reader::clear()
{
    for (CGColleV1Image *image : m_images) {
        delete image;
    }
    m_images.clear();
    m_imagesByName.clear();
    m_imageNames.clear();
}

bool CGColleV1Reader::open()
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

bool CGColleV1Reader::isValidFormat(const char *ptr)
{
    static const char CGC[4] = { 'C','G', 'C', '1' };
    if (qstrncmp(ptr, CGC, 4) != 0) {
        return false;
    }

    return true;
}

bool CGColleV1Reader::isValidFormat() const
{
    return CGColleV1Reader::isValidFormat(reinterpret_cast<const char *>(m_data));
}

bool CGColleV1Reader::isValidFormat(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    uchar *ptr = f.map(0, 4);
    if (!ptr) {
        return false;
    }

    return CGColleV1Reader::isValidFormat(reinterpret_cast<const char *>(ptr));
}

#define readNumber(ptr, dst, type) \
    dst = *reinterpret_cast<type *>(ptr); \
    ptr += sizeof(dst);
#define readUInt32(ptr, dst) readNumber(ptr, dst, uint32_t)
#define readFlags(ptr, dst) readNumber(ptr, dst, CGColleV1Reader::CGColleV1Image::Flags)
#define readString(ptr, dst) \
    { \
        uint32_t nameSize = *reinterpret_cast<uint32_t*>(ptr); \
        ptr += sizeof(nameSize); \
        QByteArray nameBuf((const char *)ptr, nameSize); \
        image->name = QString::fromUtf8(nameBuf); \
        ptr += nameSize; \
    }

bool CGColleV1Reader::scanMeta()
{
    uchar *metaPtr = m_data + 12;

    uint32_t imagesCount = *reinterpret_cast<uint32_t*>(metaPtr);
    metaPtr += sizeof(uint32_t);

    int dataOffset = 0;
    int baseImageIdx = 0;
    for (uint32_t i = 0; i < imagesCount; ++i) {
        CGColleV1Image *image = new CGColleV1Image();

        uint32_t type = 0;
        readUInt32(metaPtr, type);
        image->type = (CGColleV1Image::Type)(type);

        readString(metaPtr, image->name);
        readUInt32(metaPtr, image->fileSize);
        readUInt32(metaPtr, image->width);
        readUInt32(metaPtr, image->height);
        readUInt32(metaPtr, image->offsetX);
        readUInt32(metaPtr, image->offsetY);
        readUInt32(metaPtr, image->layerId);
        readUInt32(metaPtr, image->compositionMethod);
        readFlags(metaPtr, image->flags);

        if (image->type == CGColleV1Image::Type::BaseImage) {
            baseImageIdx = i;
        }
        image->baseImageIdx = baseImageIdx;
        image->dataOffset = dataOffset;

        m_images << image;

        // Do not expose hidden image
        if (!image->flags.testFlag(CGColleV1Image::Flag::Hide)) {
            m_imageNames << image->name;
            m_imagesByName.insert(image->name, image);
        }

        dataOffset += image->fileSize;
    }

    m_dataStart = metaPtr - m_data;

    return true;
}

void CGColleV1Reader::close()
{
    clear();
    m_file.close();
}

QByteArray CGColleV1Reader::read(const QString &imageName)
{
    auto it = m_imagesByName.find(imageName);
    if (it == m_imagesByName.end()) {
        return QByteArray();
    }

    CGColleV1Image *imageMeta = *it;
    if (imageMeta->type == CGColleV1Image::Type::BaseImage) {
        // Base frame
        return QByteArray(reinterpret_cast<const char *>(m_data + m_dataStart + imageMeta->dataOffset), imageMeta->fileSize);
    } else if (imageMeta->type == CGColleV1Image::Type::LayerImage) {
        // Layer frame
        CGColleV1Image *baseFrameMeta = m_images[imageMeta->baseImageIdx];

        QImage frame = QImage::fromData(m_data + m_dataStart + baseFrameMeta->dataOffset, baseFrameMeta->fileSize);
        QImage layerFrame = QImage::fromData(m_data + m_dataStart + imageMeta->dataOffset, imageMeta->fileSize);

        QPainter painter(&frame);
        painter.drawImage(QPointF(imageMeta->offsetX, imageMeta->offsetY), layerFrame);

        QByteArray frameData;
        QBuffer buf(&frameData);
        buf.open(QIODevice::WriteOnly);
        frame.save(&buf, "BMP");

        return frameData;
    } else {
        return QByteArray();
    }
}

QStringList CGColleV1Reader::imageNames() const
{
    return m_imageNames;
}