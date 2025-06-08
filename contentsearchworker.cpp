#include "contentsearchworker.h"
#include "contentsearchtask.h"
#include <QThreadPool>
#include <QDir>
#include <QDebug>

ContentSearchWorker::ContentSearchWorker() {}

void ContentSearchWorker::cancelSearch()
{
    cancelRequested = true;
    queueNotEmpty.wakeAll();
}

void ContentSearchWorker::startSearch(const QString &path, const QString &searchQuery)
{
    cancelRequested = false;
    enqueueDone = false;
    this->searchQuery = searchQuery;
    QThread *dirThread = QThread::create([=]()
    {
        recursiveSearch(QDir(path));
        enqueueDone = true;
        queueNotEmpty.wakeAll(); 
    });

    connect(dirThread, QThread::finished, dirThread, QThread::deleteLater);
    dirThread->start();
    startTasks(4);
}

void ContentSearchWorker::recursiveSearch(const QDir &dir)
{
    if (cancelRequested)
        return;

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);
    for (QFileInfo &entry : entries)
    {
        if (cancelRequested)
            return;

        if (entry.isDir())
        {
            recursiveSearch(QDir(entry.filePath()));
        }
        else
        {
            QMutexLocker locker(&queueMutex);
            taskQueue.enqueue(entry);
            queueNotEmpty.wakeOne();
        }
    }
}

void ContentSearchWorker::startTasks(int numThreads)
{
    for (int i = 0; i < numThreads; i++)
    {
        activeThreadCount++;
        QThreadPool::globalInstance()->start(new ContentSearchTask(this));
    }
}