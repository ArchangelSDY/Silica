#include "ImageRank.h"
#include "LocalDatabase.h"

const int ImageRank::DEFAULT_VALUE = 3;

ImageRank::ImageRank(Image *image, QObject *parent) :
    QObject(parent) ,
    m_image(image) ,
    m_inited(false) ,
    m_value(DEFAULT_VALUE)
{
}

int ImageRank::value()
{
    if (!m_inited) {
        load();
    }

    return m_value;
}

void ImageRank::setValue(int rank)
{
    m_value = rank;
    save();
    emit imageRankChanged(rank);
}

void ImageRank::load()
{
    m_value = LocalDatabase::instance()->queryImageRankValue(m_image);
    m_inited = true;
}

void ImageRank::save()
{
    LocalDatabase::instance()->updateImageRank(m_image, m_value);
}
