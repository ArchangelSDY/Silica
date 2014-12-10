#ifndef IMAGEHISTOGRAM_H
#define IMAGEHISTOGRAM_H

class ImageHistogramPrivate;

class ImageHistogram
{
    friend class ImageHistogramPrivate;
public:
    ImageHistogram(const QImage &image);
    ~ImageHistogram();

    void calc();
    double compare(const ImageHistogram &other) const;

private:
    ImageHistogramPrivate *m_p;
};

#endif // IMAGEHISTOGRAM_H
