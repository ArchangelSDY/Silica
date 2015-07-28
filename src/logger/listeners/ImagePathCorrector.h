#ifndef IMAGEPATHCORRECTOR_H
#define IMAGEPATHCORRECTOR_H

#include <QList>
#include <QMutex>
#include <QQueue>
#include <QUrl>

#include "logger/Logger.h"

class QThread;
class Navigator;

class ImagePathCorrector : public Logger::LogListener
{
private:
    class WorkerThread;

public:
    class PathPatch
    {
    public:
        PathPatch();

        QString toString() const;

        QString imageHashStr;
        QUrl oldImageUrl;
        QUrl newImageUrl;
        bool shouldApply;
    };


    class PromptClient
    {
    public:
        virtual void prompt(QList<ImagePathCorrector::PathPatch> &patches) = 0;
        virtual ~PromptClient() {}
    };


    ImagePathCorrector(const QList<QString> &searchDirs,
                       Navigator *navigator,
                       PromptClient *promptClient);
    ~ImagePathCorrector();

    void dispatch(const LogRecord &record) override;

    void setTestMode(bool isTestMode);

private:
    void promptPatches();
    void applyPatches();
    void reloadPlayList();

    QList<QString> m_searchDirs;
    Navigator *m_navigator;
    QQueue<LogRecord> m_incomingRecords;
    QMutex m_recordsMutex;
    ImagePathCorrector::WorkerThread *m_worker;
    QList<ImagePathCorrector::PathPatch> m_pathPatches;
    PromptClient *m_promptClient;

    bool m_isTestMode;
};

#endif // IMAGEPATHCORRECTOR_H
