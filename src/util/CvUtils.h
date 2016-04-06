#ifndef CVUTILS_H
#define CVUTILS_H

#include <QImage>
#include <opencv2/core/core.hpp>

namespace CvUtils {
    cv::Mat QImageToCvMat(const QImage &image);
}

#endif // CVUTILS_H
