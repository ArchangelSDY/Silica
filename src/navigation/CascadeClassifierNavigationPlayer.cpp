#include "CascadeClassifierNavigationPlayer.h"

#include <QFile>
#include <QRect>
#include <QTime>
#include <QWidget>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "Navigator.h"
#include "util/CvUtils.h"

CascadeClassifierNavigationPlayer::CascadeClassifierNavigationPlayer(
        Navigator *navigator, QWidget *view, QObject *parent) :
    AbstractNavigationPlayer(navigator, parent) ,
    m_view(view) ,
    m_classifier(nullptr) ,
    m_curRegionIndex(0)
{
}

CascadeClassifierNavigationPlayer::~CascadeClassifierNavigationPlayer()
{
    if (m_classifier) {
        delete m_classifier;
    }
}

void CascadeClassifierNavigationPlayer::goNext()
{
    if (m_curRegionIndex >= m_regions.size() - 1) {
        m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() + 1, 1);

        m_curRegionIndex = 0;
        detectRegions();
    } else {
        m_curRegionIndex ++;
    }

    focusOnCurrentRegion();
}

void CascadeClassifierNavigationPlayer::goPrev()
{
    if (m_curRegionIndex <= 0) {
        m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() - 1, -1);

        detectRegions();
        m_curRegionIndex = m_regions.size() - 1;
    } else {
        m_curRegionIndex --;
    }

    focusOnCurrentRegion();
}

void CascadeClassifierNavigationPlayer::onEnter()
{
    if (m_classifier) {
        return;
    }

    QFile f(":/assets/lbpcascade_animeface.xml");
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("Fail to load cascade classifier");
        return;
    }

    QByteArray content = f.readAll();
    cv::FileStorage cvFile(content.toStdString(), cv::FileStorage::READ | cv::FileStorage::MEMORY);
    if (!cvFile.isOpened()) {
        qWarning("Fail to load cascade classifier as cv::FileStorage");
        return;
    }

    cv::CascadeClassifier *classifier = new cv::CascadeClassifier();
    if (!classifier->read(cvFile.getFirstTopLevelNode())) {
        qWarning("Fail to decode cascade classifer");
        delete classifier;
        return;
    }

    m_classifier = classifier;
}

void CascadeClassifierNavigationPlayer::detectRegions()
{
    if (!m_classifier) {
        return;
    }

    Image *curImage = m_navigator->currentImage();
    cv::Mat cvImage = CvUtils::QImageToCvMat(curImage->data());

    cv::Mat grayImage;
    cv::cvtColor(cvImage, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat eqGrayImage;
    cv::equalizeHist(grayImage, eqGrayImage);

    m_regions.clear();
    std::vector<cv::Rect> regions;
    m_classifier->detectMultiScale(eqGrayImage, regions, 1.1, 5, 0,
                                   cv::Size(150, 150));

    for (const cv::Rect &region : regions) {
        m_regions << QRect(region.x, region.y, region.width, region.height);
    }
}

void CascadeClassifierNavigationPlayer::focusOnCurrentRegion()
{
    if (!m_view) {
        return;
    }

    if (m_curRegionIndex < m_regions.size() && m_curRegionIndex >= 0) {
        const QRect &rect = m_regions[m_curRegionIndex];

        // Fit to view size
        QSize size = rect.size().scaled(m_view->size(), Qt::KeepAspectRatio);
        QPoint center = rect.center();
        QRect fittedRect(center.x() - size.width() / 2,
                         center.y() - size.height() / 2,
                         size.width(),
                         size.height());

        m_navigator->focusOnRect(fittedRect);
    }
}
