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
    static const int MIN_VALUE;
    static const int MAX_VALUE;
    static const int MIN_SHOWN_VALUE;

    int value();
    void setValue(int rank);

    void upVote();
    void downVote();

    Image *image() { return m_image; }

signals:
    void imageRankChanged(int rank);

private:
    void load();
    void save();
    int adjustNewRank(int newRank);

    Image *m_image;
    bool m_inited;
    int m_value;

};

#endif // IMAGERANK_H
