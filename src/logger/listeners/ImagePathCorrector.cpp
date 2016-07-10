#include "ImagePathCorrector.h"

#include <QFileInfo>
#include <QMutexLocker>
#include <QString>
#include <QThread>
#include <QUrl>

#include "db/LocalDatabase.h"
#include "image/ImageSourceManager.h"
#include "Navigator.h"

ImagePathCorrector::PathPatch::PathPatch() : shouldApply(true)
{
}

QString ImagePathCorrector::PathPatch::toString() const
{
    QString oldUrl = oldImageUrl.toString();
    QString newUrl = newImageUrl.toString();

    QStringList oldUrlParts = oldUrl.split('/');
    QStringList newUrlParts = newUrl.split('/');

    int commonPrefixCnt = 0;
    int minLength = qMin(oldUrlParts.length(), newUrlParts.length());
    for (; commonPrefixCnt < minLength; ++commonPrefixCnt ) {
        if (oldUrlParts[commonPrefixCnt] != newUrlParts[commonPrefixCnt]) {
            break;
        }
    }

    int commonSuffixCnt = 0;
    for (; commonSuffixCnt < minLength; ++commonSuffixCnt) {
        int oldIdx = oldUrlParts.length() - commonSuffixCnt;
        int newIdx = newUrlParts.length() - commonSuffixCnt;
        if (oldUrlParts[oldIdx - 1] != newUrlParts[newIdx - 1]) {
            break;
        }
    }

    QList<QString> commonPrefixParts = oldUrlParts.mid(0, commonPrefixCnt);
    QList<QString> commonSuffixParts = oldUrlParts.mid(oldUrlParts.length() - commonSuffixCnt);

    QString oldDiff = QStringList(oldUrlParts.mid(commonPrefixCnt, oldUrlParts.length() - commonPrefixCnt - commonSuffixCnt))
        .join('/');
    QString newDiff = QStringList(newUrlParts.mid(commonPrefixCnt, newUrlParts.length() - commonPrefixCnt - commonSuffixCnt))
        .join('/');

    QString diff = QString("{ %1 => %2 }")
        .arg(oldDiff, newDiff);

    QStringList showTextParts;
    showTextParts << commonPrefixParts << diff << commonSuffixParts;
    QString showText = showTextParts.join('/');

    return showText;
}


class ImagePathCorrector::WorkerThread : public QThread
{
public:
    WorkerThread(ImagePathCorrector &corrector);

    void notifyExit();

protected:
    void run() override;

private:
    void correctPath(const LogRecord &record);
    QUrl searchNewUrl(const QUrl &oldUrl);

    static const int SLEEP_MSECS = 100;
    static const int MAX_IDLE_LOOP_BEFORE_PROMPT_FIX = 30;

    ImagePathCorrector &m_corrector;
    bool m_shouldExit;
    int m_curIdleLoop = 0;
    QHash<QUrl, bool> m_handledUrls;
};

ImagePathCorrector::WorkerThread::WorkerThread(ImagePathCorrector &corrector)
    : m_corrector(corrector)
    , m_shouldExit(false)
{
}

void ImagePathCorrector::WorkerThread::notifyExit()
{
    m_shouldExit = true;
}

void ImagePathCorrector::WorkerThread::run()
{
    while (!m_shouldExit) {
        m_corrector.m_recordsMutex.lock();
        if (m_corrector.m_incomingRecords.isEmpty()) {
            m_corrector.m_recordsMutex.unlock();

            if (m_curIdleLoop >= MAX_IDLE_LOOP_BEFORE_PROMPT_FIX) {
                if (!m_corrector.m_pathPatches.isEmpty()) {
                    // Show a dialog for user to check patches
                    m_corrector.promptPatches();

                    // Apply patches to database
                    m_corrector.applyPatches();

                    // Reload playlist
                    m_corrector.reloadPlayList();
                }

                m_curIdleLoop = 0;
            } else {
                m_curIdleLoop++;
            }

            if (!m_corrector.m_isTestMode) {
                QThread::msleep(SLEEP_MSECS);
            }
        } else {
            LogRecord record = m_corrector.m_incomingRecords.dequeue();
            m_corrector.m_recordsMutex.unlock();

            correctPath(record);
        }
    }
}

void ImagePathCorrector::WorkerThread::correctPath(const LogRecord &record)
{
    QString imageHashStr = record.extra.value("ImageHashStr").toString();
    QUrl imageUrl = record.extra.value("ImageURL").toUrl();

    if (m_handledUrls.contains(imageUrl)) {
        return;
    } else {
        m_handledUrls.insert(imageUrl, true);
    }

    ImageSource *source = ImageSourceManager::instance()->createSingle(imageUrl);
    if (source && !source->exists()) {
        delete source;

        QUrl newImageUrl = searchNewUrl(imageUrl);
        if (!newImageUrl.isEmpty()) {
            ImagePathCorrector::PathPatch patch;
            patch.imageHashStr = imageHashStr;
            patch.oldImageUrl = imageUrl;
            patch.newImageUrl = newImageUrl;

            m_corrector.m_pathPatches << patch;
        }
    }
}

QUrl ImagePathCorrector::WorkerThread::searchNewUrl(const QUrl &oldUrl)
{
    QUrl fileUrl = oldUrl;
    fileUrl.setScheme("file");
    QString oldPath = fileUrl.toLocalFile();
    QStringList oldPathParts = oldPath.split('/');

    // Try until the deepest path
    for (int i = oldPathParts.length() - 1; i >= 0; --i) {
        QStringList matchOldPathParts = oldPathParts.mid(i);
        QString matchOldPath = matchOldPathParts.join('/');

        foreach (const QString &dir, m_corrector.m_searchDirs) {
            QString fixedSourcePath = dir + '/' + matchOldPath;

            QUrl newUrl = QUrl::fromLocalFile(fixedSourcePath);
            newUrl.setScheme(oldUrl.scheme());
            newUrl.setFragment(oldUrl.fragment());

            ImageSource *source = ImageSourceManager::instance()->createSingle(newUrl);
            if (source && source->exists()) {
                delete source;
                return newUrl;
            }
        }
    }

    return QUrl();
}



ImagePathCorrector::ImagePathCorrector(const QList<QString> &searchDirs,
                                       Navigator *navigator,
                                       PromptClient *promptClient)
    : m_searchDirs(searchDirs)
    , m_navigator(navigator)
    , m_promptClient(promptClient)
    , m_isTestMode(false)
{
    m_worker = new ImagePathCorrector::WorkerThread(*this);
    m_worker->start();
}

ImagePathCorrector::~ImagePathCorrector()
{
    m_worker->notifyExit();
    m_worker->wait();
    delete m_worker;

    if (m_promptClient) {
        delete m_promptClient;
    }
}

void ImagePathCorrector::dispatch(const LogRecord &record)
{
    QMutexLocker locker(&m_recordsMutex);
    m_incomingRecords.enqueue(record);
}

void ImagePathCorrector::promptPatches()
{
    if (!m_promptClient) {
        return;
    }

    m_promptClient->prompt(m_pathPatches);
}

void ImagePathCorrector::applyPatches()
{
    foreach (const ImagePathCorrector::PathPatch &patch, m_pathPatches) {
        if (patch.shouldApply) {
            LocalDatabase::instance()->updateImageUrl(patch.oldImageUrl,
                                                      patch.newImageUrl);
        }
    }
    m_pathPatches.clear();
}

void ImagePathCorrector::reloadPlayList()
{
    if (m_navigator) {
        QSharedPointer<PlayList> pl = m_navigator->playList();
        if (pl) {
            PlayListRecord *plr = pl->record();
            if (plr) {
                plr->reload();
            }
        }
    }
}

void ImagePathCorrector::setTestMode(bool isTestMode)
{
    m_isTestMode = isTestMode;
}
