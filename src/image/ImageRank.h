#ifndef IMAGERANK_H
#define IMAGERANK_H

#include <QObject>

class Image;

class ImageRank : public QObject
{
    Q_OBJECT
public:
    explicit ImageRank(Image *image, QObject *parent = 0);

    static const int DEFAULT_VALUE;

    int value();
    void setValue(int rank);

    Image *image() { return m_image; }

signals:
    void imageRankChanged(int rank);

private:
    void load();
    void save();

    Image *m_image;
    bool m_inited;
    int m_value;

};

#endif // IMAGERANK_H
