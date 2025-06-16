#ifndef CONTENTSEARCHWORKER_H
#define CONTENTSEARCHWORKER_H

#include <QObject>
#include <QFileInfo>
#include "abstractsearchworker.h"
#include <QSemaphore>
#include <QThreadPool>
#include <QMutex>
#include <QWaitCondition>
#include "msqueue.h"

class ContentSearchWorker : public AbstractSearchWorker
{
    Q_OBJECT
public:
    ContentSearchWorker();

    // QAtomicInt cancelRequested;
    // QAtomicInt enqueueDone;
    // QQueue<QFileInfo> taskQueue;
    // QMutex queueMutex;
    // QWaitCondition queueNotEmpty;

    // int activeThreadCount = 0;

    MSQueue<QFileInfo> taskQueue;
    std::atomic<bool> cancelRequested = false;
    std::atomic<bool> enqueueDone = false;
    std::atomic<int> activeThreadCount = 0;
    QString searchQuery;
    QSemaphore taskAvailable;

public slots:
    void startSearch(const QString &path, const QString &searchQuery) override;
    void cancelSearch() override;

private:
    void recursiveSearch(const QDir &dir);
    void startTasks(int numThreads);

    
};

class ContentSearchTask : public QRunnable
{
public:
    ContentSearchTask(ContentSearchWorker* worker);

protected:
    void run() override;

private:
    ContentSearchWorker* worker;
};

#endif // CONTENTSEARCHWORKER_H
