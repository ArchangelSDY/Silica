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

class CGColleV1ImageReader : public CGColleReader::ImageReader
{
public:
    CGColleV1ImageReader(const QString &packagePath,
                         CGColleV1Reader::CGColleV1Image::Type type,
                         uint32_t dataPos,
                         uint32_t dataSize,
                         uint32_t baseDataPos,
                         uint32_t baseDataSize,
                         uint32_t offsetX,
                         uint32_t offsetY) :
        m_packagePath(packagePath),
        m_type(type),
        m_dataPos(dataPos),
        m_dataSize(dataSize),
        m_baseDataPos(baseDataPos),
        m_baseDataSize(baseDataSize),
        m_offsetX(offsetX),
        m_offsetY(offsetY)
    {
    }
    
    virtual QByteArray read() override;

private:
    QString m_packagePath;
    CGColleV1Reader::CGColleV1Image::Type m_type;
    uint32_t m_dataPos;
    uint32_t m_dataSize;
    uint32_t m_baseDataPos;
    uint32_t m_baseDataSize;
    uint32_t m_offsetX;
    uint32_t m_offsetY;
};

QByteArray CGColleV1ImageReader::read()
{
    QFile f(m_packagePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }

    if (m_type == CGColleV1Reader::CGColleV1Image::Type::BaseImage) {
        // Base frame
        f.seek(m_dataPos);
        return f.read(m_dataSize);
    } else if (m_type == CGColleV1Reader::CGColleV1Image::Type::LayerImage) {
        // Layer frame
        f.seek(m_baseDataPos);
        QImage frame = QImage::fromData(f.read(m_baseDataSize));

        f.seek(m_dataPos);
        QImage layerFrame = QImage::fromData(f.read(m_dataSize));

        QPainter painter(&frame);
        painter.drawImage(QPointF(m_offsetX, m_offsetY), layerFrame);

        QByteArray frameData;
        QBuffer buf(&frameData);
        buf.open(QIODevice::WriteOnly);
        frame.save(&buf, "BMP");

        return frameData;
    } else {
        return QByteArray();
    }
}

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
    return CGColleV1Reader::isValidFormat(m_file.fileName());
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

#define readNumber(dst) \
    m_file.read((char *)&dst, sizeof(dst));
#define readString(dst) \
    { \
        uint32_t nameSize = 0; \
        readNumber(nameSize); \
        QByteArray nameBuf = m_file.read(nameSize); \
        dst = QString::fromUtf8(nameBuf); \
    }

bool CGColleV1Reader::scanMeta()
{
    m_file.seek(12);

    uint32_t imagesCount = 0;
    readNumber(imagesCount);

    int dataOffset = 0;
    int baseImageIdx = 0;
    for (uint32_t i = 0; i < imagesCount; ++i) {
        CGColleV1Image *image = new CGColleV1Image();

        readNumber(image->type);

        readString(image->name);
        readNumber(image->fileSize);
        readNumber(image->width);
        readNumber(image->height);
        readNumber(image->offsetX);
        readNumber(image->offsetY);
        readNumber(image->layerId);
        readNumber(image->compositionMethod);
        readNumber(image->flags);

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

    m_dataStart = m_file.pos();

    return true;
}

void CGColleV1Reader::close()
{
    clear();
    m_file.close();
}

CGColleReader::ImageReader *CGColleV1Reader::createReader(const QString &imageName)
{
    auto it = m_imagesByName.find(imageName);
    if (it == m_imagesByName.end()) {
        return nullptr;
    }

    CGColleV1Image *imageMeta = *it;
    if (imageMeta->type == CGColleV1Image::Type::BaseImage) {
        return new CGColleV1ImageReader(
            m_file.fileName(), CGColleV1Image::Type::BaseImage,
            m_dataStart + imageMeta->dataOffset,
            imageMeta->fileSize,
            0,
            0,
            0,
            0);
    } else if (imageMeta->type == CGColleV1Image::Type::LayerImage) {
        CGColleV1Image *baseFrameMeta = m_images[imageMeta->baseImageIdx];
        return new CGColleV1ImageReader(
            m_file.fileName(), CGColleV1Image::Type::LayerImage,
            m_dataStart + imageMeta->dataOffset,
            imageMeta->fileSize,
            m_dataStart + baseFrameMeta->dataOffset,
            m_dataStart + baseFrameMeta->fileSize,
            imageMeta->offsetX,
            imageMeta->offsetY);
    } else {
        return nullptr;
    }
}

QStringList CGColleV1Reader::imageNames() const
{
    return m_imageNames;
}
