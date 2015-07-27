#include "ImagePathCorrector.h"

#include <QDir>
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
            continue;
        }

        LogRecord record = m_corrector.m_incomingRecords.dequeue();
        m_corrector.m_recordsMutex.unlock();

        correctPath(record);
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
    QString oldPath = oldUrl.path();
    QStringList oldPathParts = oldPath.split(QDir::separator());

    // Try until the deepest path
    for (int i = oldPathParts.length() - 1; i >= 0; --i) {
        QStringList matchOldPathParts = oldPathParts.mid(i);
        QString matchOldPath = matchOldPathParts.join(QDir::separator());

        foreach (const QString &dir, m_corrector.m_searchDirs) {
            QString fixedSourcePath = dir + QDir::separator() + matchOldPath;
            QUrl newUrl(oldUrl);
            newUrl.setPath(fixedSourcePath);

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
        PlayList *pl = m_navigator->playList();
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
