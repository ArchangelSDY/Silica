#include "CGColleV1Reader.h"

#include <QBuffer>
#include <QDataStream>
#include <QImage>
#include <QPainter>

class CGColleV1Reader::CGColleV1Frame
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
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    Type type;
    int dataOffset;
    QString scene;
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

Q_DECLARE_OPERATORS_FOR_FLAGS(CGColleV1Reader::CGColleV1Frame::Flags)

class CGColleV1Reader::CGColleV1Image
{
public:
    QString name;
    QVector<int> frameIndices;
};

class CGColleV1ImageReader : public CGColleReader::ImageReader
{
public:
    CGColleV1ImageReader(const QString &packagePath,
                         uint32_t dataStart,
                         const QList<CGColleV1Reader::CGColleV1Frame> &frames) :
        m_packagePath(packagePath),
        m_dataStart(dataStart) ,
        m_frames(frames)

    {
    }
    
    virtual QByteArray read() override;

private:
    QString m_packagePath;
    uint32_t m_dataStart;
    QList<CGColleV1Reader::CGColleV1Frame> m_frames;
};

class CGColleV1Reader::CGColleV1CompositeRule
{
public:
    static const int TYPE = 0;

    virtual ~CGColleV1CompositeRule() {}

    virtual QList<CGColleV1Image> createImages(const QList<CGColleV1Frame *> &allFrames,
                                               const QList<int> &frameIndices) = 0;
};

class CGColleV1CartesianProductRule : public CGColleV1Reader::CGColleV1CompositeRule
{
public:
    CGColleV1CartesianProductRule(const QList<QPair<uint8_t, uint8_t> > &layerSelectRanges) :
        m_layerSelectRanges(layerSelectRanges)
    {
    }

    virtual QList<CGColleV1Reader::CGColleV1Image> createImages(const QList<CGColleV1Reader::CGColleV1Frame *> &allFrames,
                                                                const QList<int> &frameIndices) override
    {
        QList<CGColleV1Reader::CGColleV1Image> ret;

        QVector<QVector<int> > indicesGroupByLayer(5);
        for (const int index : frameIndices) {
            uchar layerId = allFrames[index]->layerId;
            if (layerId >= indicesGroupByLayer.size()) {
                indicesGroupByLayer.resize(layerId + 1);
            }
            indicesGroupByLayer[layerId].push_back(index);
        }

        if (indicesGroupByLayer.empty()) {
            return ret;
        }

        static const int EMPTY_INDEX = -1;

        for (int i = 0; i < m_layerSelectRanges.count(); i++) {
            // TODO: Support larger range
            // This is a very limited combination implementation for now.
            // Only [0, 1] and [1, 1] are supported.
            if (m_layerSelectRanges[i].first == 0) {
                indicesGroupByLayer[i].prepend(EMPTY_INDEX);
            }
        }

        QVector<QVector<int> > frameIndicesForEachImage;
        for (const int &val : indicesGroupByLayer[0]) {
            frameIndicesForEachImage.push_back(QVector<int>{val});
        }
        for (int i = 1; i < indicesGroupByLayer.count(); i++) {
            if (!indicesGroupByLayer[i].isEmpty()) {
                frameIndicesForEachImage = product(frameIndicesForEachImage, indicesGroupByLayer[i]);
            }
        }

        for (const auto &indices : frameIndicesForEachImage) {
            CGColleV1Reader::CGColleV1Image image;
            QStringList frameNames;

            for (const int &index : indices) {
                if (index != EMPTY_INDEX) {
                    CGColleV1Reader::CGColleV1Frame *frame = allFrames[index];
                    frameNames << frame->name;
                }
            }

            // TODO: Better name
            image.name = frameNames.join('+');
            image.frameIndices = indices;

            ret << image;
        }

        return ret;
    }

    QVector<QVector<int> > product(const QVector<QVector<int> > &current, const QVector<int> &next)
    {
        QVector<QVector<int> > ret;

        for (const auto &set : current) {
            for (const int &val : next) {
                QVector<int> newSet = set;
                newSet.push_back(val);
                ret.push_back(newSet);
            }
        }

        return ret;
    }

private:
    QList<QPair<uint8_t, uint8_t> > m_layerSelectRanges;
};

QByteArray CGColleV1ImageReader::read()
{
    QFile f(m_packagePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }

    Q_ASSERT(m_frames.count() > 0);

    const CGColleV1Reader::CGColleV1Frame &baseFrame = m_frames[0];
    f.seek(m_dataStart + baseFrame.dataOffset);
    QImage image = QImage::fromData(f.read(baseFrame.fileSize));

    QPainter painter(&image);
    for (int i = 1; i < m_frames.count(); i++) {
        const CGColleV1Reader::CGColleV1Frame &layerFrame = m_frames[i];

        f.seek(m_dataStart + layerFrame.dataOffset);
        QImage layerImage = QImage::fromData(f.read(layerFrame.fileSize));

        painter.drawImage(QPointF(layerFrame.offsetX, layerFrame.offsetY), layerImage);
    }

    QByteArray imageData;
    QBuffer buf(&imageData);
    buf.open(QIODevice::WriteOnly);
    image.save(&buf, "BMP");

    return imageData;
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
    for (CGColleV1Frame *image : m_frames) {
        delete image;
    }
    m_frames.clear();
    m_imagesByName.clear();
    m_imageNames.clear();
    for (CGColleV1CompositeRule *rule : m_rules) {
        delete rule;
    }
    m_rules.clear();
}

bool CGColleV1Reader::open()
{
    if (!m_file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Check format and scan meta
    if (!isValidFormat() || !scan()) {
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

bool CGColleV1Reader::scan()
{
    // Skip header
    m_file.seek(8);

    while (!m_file.atEnd()) {
        QByteArray chunkType = m_file.read(4);
        if (chunkType == "META") {
            if (!scanMeta()) {
                return false;
            }
        } else if (chunkType == "CPRL") {
            if (!scanCompositeRules()) {
                return false;
            }
        } else if (chunkType == "DATA") {
            uint32_t chunkLength = 0;
            readNumber(chunkLength);

            m_dataStart = m_file.pos();

            // Skip data for now
            m_file.seek(m_file.pos() + chunkLength);
        } else {
            // Unknown chunk, skip
            uint32_t chunkLength = 0;
            readNumber(chunkLength);
            m_file.seek(m_file.pos() + chunkLength);
        }
    }

    // Generate composited plans
    for (auto it = m_frameIndicesGroupByScene.constBegin(); it != m_frameIndicesGroupByScene.constEnd(); it++) {
        auto ruleIt = m_rules.find(it.key());
        if (ruleIt == m_rules.end()) {
            ruleIt = m_rules.find("*");
        }
        Q_ASSERT(ruleIt != m_rules.end());

        auto images = ruleIt.value()->createImages(m_frames, it.value());
        for (const auto &image : images) {
            m_imageNames << image.name;
            m_imagesByName.insert(image.name, image);
        }
    }

    return true;
}

bool CGColleV1Reader::scanMeta()
{
    uint32_t metaLength = 0;
    readNumber(metaLength);

    uint32_t imagesCount = 0;
    readNumber(imagesCount);

    int dataOffset = 0;
    for (uint32_t i = 0; i < imagesCount; ++i) {
        CGColleV1Frame *image = new CGColleV1Frame();

        readNumber(image->type);

        readString(image->scene);
        readString(image->name);
        readNumber(image->fileSize);
        readNumber(image->width);
        readNumber(image->height);
        readNumber(image->offsetX);
        readNumber(image->offsetY);
        readNumber(image->layerId);
        readNumber(image->compositionMethod);
        readNumber(image->flags);

        image->dataOffset = dataOffset;

        m_frameIndicesGroupByScene[image->scene] << i;
        m_frames << image;

        dataOffset += image->fileSize;
    }

    return true;
}

bool CGColleV1Reader::scanCompositeRules()
{
    qint64 cprlStart = m_file.pos();

    uint32_t cprlLength = 0;
    readNumber(cprlLength);

    uint32_t rulesCount = 0;
    readNumber(rulesCount);

    for (uint32_t i = 0; i < rulesCount; ++i) {
        QString matcher;
        readString(matcher);

        uint8_t type;
        readNumber(type);

        if (type == CGColleV1CartesianProductRule::TYPE) {
            // Cartesian Product Rule
            uint8_t count;
            readNumber(count);

            QList<QPair<uint8_t, uint8_t> > layerRanges;
            for (uint8_t j = 0; j < count; ++j) {
                uint8_t min;
                readNumber(min);
                uint8_t max;
                readNumber(max);
                layerRanges << QPair<uint8_t, uint8_t>(min, max);
            }

            m_rules.insert(matcher, new CGColleV1CartesianProductRule(layerRanges));
        } else {
            // Unknown rule
            // Skip all remaining CPRL chunk data
            m_file.seek(cprlStart + 8 + cprlLength);
            return false;
        }
    }

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

    QList<CGColleV1Frame> frames;
    for (const int &index : it->frameIndices) {
        frames << *m_frames[index];
    }

    return new CGColleV1ImageReader(m_file.fileName(), m_dataStart, frames);
}

QStringList CGColleV1Reader::imageNames() const
{
    return m_imageNames;
}
