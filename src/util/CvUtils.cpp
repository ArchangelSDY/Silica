#include "CvUtils.h"

#include <QDebug>
#include <QImage>

namespace CvUtils {

cv::Mat QImageToCvMat(const QImage &image)
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
        // IMPORTANT: We MUST return a cloned one because the Mat is a `borrowed` pointer
        // from `npImage` while `npImage` will be destroyed when function returns
        return cv::Mat(npImage.height(), npImage.width(), CV_8UC4,
                       const_cast<uchar *>(npImage.bits()),
                       npImage.bytesPerLine()).clone();
    }
    case QImage::Format_RGB888:
    {
        QImage swapped = image.rgbSwapped();
        // IMPORTANT: We MUST return a cloned one because the Mat is a `borrowed` pointer
        // from `swapped` while `swapped` will be destroyed when function returns
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
        qWarning() << "CvUtils: unknown image format"
                   << image.format();
        break;
    }

    return cv::Mat();
}

}
