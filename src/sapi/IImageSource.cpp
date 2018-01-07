#include "IImageSource.h"

namespace sapi{

IImageSource::~IImageSource()
{
}

QVariantHash IImageSource::readMetadata()
{
    return QVariantHash();
}

}
