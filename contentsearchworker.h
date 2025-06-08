#ifndef CONTENTSEARCHWORKER_H
#define CONTENTSEARCHWORKER_H

#include <QObject>
#include <QFileInfo>
#include "abstractsearchworker.h"
#include <QQueue>
#include <QThreadPool>
#include <QMutex>
#include <QWaitCondition>

class ContentSearchWorker : public AbstractSearchWorker
{
    Q_OBJECT
public:
    ContentSearchWorker();

    QAtomicInt cancelRequested;
    QAtomicInt enqueueDone;
    QQueue<QFileInfo> taskQueue;
    QMutex queueMutex;
    QWaitCondition queueNotEmpty;

    int activeThreadCount = 0;

    QString searchQuery;

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
