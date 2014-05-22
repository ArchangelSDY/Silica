#ifndef IMAGEHOTSPOT_H
#define IMAGEHOTSPOT_H

#include <QRect>

class Image;

class ImageHotspot
{
public:
    ImageHotspot(Image *image, const QRect &rect);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }
    Image *image() { return m_image; }
    QRect rect() const { return m_rect; }

    bool save();
    bool remove();

private:
    Image *m_image;
    QRect m_rect;
    int m_id;
};

#endif // IMAGEHOTSPOT_H
