#include "IImageSourceFactory.h"

#include <QFileInfo>

namespace sapi {

IImageSourceFactory::~IImageSourceFactory()
{
}

bool IImageSourceFactory::isValidFileName(const QString &name)
{
    QFileInfo fileInfo(name);
    return fileNameSuffixes().contains(fileInfo.suffix());
}

}
