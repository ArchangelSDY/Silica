#pragma once

#include <QFile>
#include <QHash>
#include <QList>
#include <QMap>

#include "CGColleReader.h"

class CGColleV1Reader : public CGColleReader
{
public:
    class CGColleV1Frame;
    class CGColleV1Image;
    class CGColleV1CompositeRule;

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
    bool scan();
    bool scanMeta();
    bool scanCompositeRules();
    void clear();

    QFile m_file;
    uint32_t m_dataStart;
    QList<CGColleV1Frame *> m_frames;
    QMap<QString, QList<int> > m_frameIndicesGroupByScene;
    QStringList m_imageNames;
    QHash<QString, CGColleV1Image> m_imagesByName;
    QHash<QString, CGColleV1CompositeRule *> m_rules;
};
