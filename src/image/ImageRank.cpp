#include "ImageRank.h"

#include "db/LocalDatabase.h"

const int ImageRank::DEFAULT_VALUE = 3;
const int ImageRank::MIN_VALUE = 1;
const int ImageRank::MAX_VALUE = 5;
const int ImageRank::MIN_SHOWN_VALUE = 3;

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
    rank = adjustNewRank(rank);
    if (m_value != rank) {
        m_value = rank;
        save();
        emit imageRankChanged(rank);
    }
}

void ImageRank::upVote()
{
    setValue(value() + 1);
}

void ImageRank::downVote()
{
    setValue(value() - 1);
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

int ImageRank::adjustNewRank(int newRank)
{
    newRank = qMin(MAX_VALUE, newRank);
    newRank = qMax(MIN_VALUE, newRank);
    return newRank;
}
