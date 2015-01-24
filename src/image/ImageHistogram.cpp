#include <QDebug>
#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ImageHistogram.h"

class ImageHistogramPrivate
{
    friend class ImageHistogram;
public:
    ImageHistogramPrivate(ImageHistogram *q, const QImage &image);

    void calc();
    double compare(const ImageHistogramPrivate &other) const;

private:
    cv::Mat imageToCvMat(const QImage &image);

    ImageHistogram *m_q;
    QImage m_image;
    cv::Mat m_hist;
};

ImageHistogramPrivate::ImageHistogramPrivate(ImageHistogram *q,
                                             const QImage &image) :
    m_q(q) ,
    m_image(image)
{
}

cv::Mat ImageHistogramPrivate::imageToCvMat(const QImage &image)
{
    switch (image.format())
    {
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    {
        cv::Mat mat(image.height(), image.width(), CV_8UC4,
                    const_cast<uchar *>(image.bits()), image.bytesPerLine());
        return mat;
    }
    case QImage::Format_ARGB32_Premultiplied:
    {
        QImage npImage = image.convertToFormat(QImage::Format_ARGB32);
        cv::Mat mat(npImage.height(), npImage.width(), CV_8UC4,
                    const_cast<uchar *>(npImage.bits()),
                    npImage.bytesPerLine());
        return mat;
    }
    case QImage::Format_RGB888:
    {
        QImage swapped = image.rgbSwapped();
        return cv::Mat(swapped.height(), swapped.width(), CV_8UC3,
                       const_cast<uchar *>(swapped.bits()),
                       swapped.bytesPerLine()).clone();
    }
    case QImage::Format_Indexed8:
    {
        cv::Mat mat(image.height(), image.width(), CV_8UC1,
                    const_cast<uchar *>(image.bits()), image.bytesPerLine());
        return mat;
    }
    default:
        qWarning() << "ImageHistogramPrivate: unknown image format"
                   << image.format();
        break;
    }

    return cv::Mat();
}

void ImageHistogramPrivate::calc()
{
    if (m_hist.data != NULL) {
        return;
    }

    cv::Mat cvImage = imageToCvMat(m_image);
    if (cvImage.data == NULL) {
        return;
    }

    cv::Mat hsv;
    cvtColor(cvImage, hsv, cv::COLOR_BGR2HSV);

    int hBins = 50;
    int sBins = 60;
    int histSize[] = { hBins, sBins };
    float hRanges[] = { 0, 180 };
    float sRanges[] = { 0, 256 };
    const float *ranges[] = { hRanges, sRanges };
    int channels[] = { 0, 1};

    cv::calcHist(&hsv, 1, channels, cv::Mat(), m_hist, 2, histSize,
                 ranges, true, false);
    cv::normalize(m_hist, m_hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
}

double ImageHistogramPrivate::compare(const ImageHistogramPrivate &other) const
{
    if (m_hist.data == NULL) {
        m_q->calc();
    }

    if (other.m_hist.data == NULL) {
        other.m_q->calc();
    }

    if (m_hist.data == NULL || other.m_hist.data == NULL) {
        return 0;
    }

    return cv::compareHist(m_hist, other.m_hist, 0);
}


ImageHistogram::ImageHistogram(const QImage &image) :
    m_p(new ImageHistogramPrivate(this, image))
{
}

ImageHistogram::~ImageHistogram()
{
    delete m_p;
}

void ImageHistogram::calc()
{
    m_p->calc();
}

double ImageHistogram::compare(const ImageHistogram &other) const
{
    return m_p->compare(*(other.m_p));
}
