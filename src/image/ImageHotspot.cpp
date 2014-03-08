#include "ImageHotspot.h"
#include "LocalDatabase.h"

ImageHotspot::ImageHotspot(Image *image, const QRect &rect) :
    m_image(image) ,
    m_rect(rect)
{
}

bool ImageHotspot::save()
{
    bool ok = LocalDatabase::instance()->insertImageHotspot(this);
    m_image->loadHotspots();
    return ok;
}

bool ImageHotspot::remove()
{
    bool ok = LocalDatabase::instance()->removeImageHotspot(this);
    m_image->loadHotspots();
    return ok;
}
