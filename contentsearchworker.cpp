#include "contentsearchworker.h"
#include <QThreadPool>
#include <QDir>
#include <QDebug>

ContentSearchWorker::ContentSearchWorker() {}

void ContentSearchWorker::cancelSearch()
{
    cancelRequested = true;
    int threadsToWake = QThread::idealThreadCount();
    taskAvailable.release(threadsToWake);
}

void ContentSearchWorker::startSearch(const QString &path, const QString &searchQuery)
{
    cancelRequested = false;
    enqueueDone = false;
    this->searchQuery = searchQuery;
    QThread *dirThread = QThread::create([this, path]()
    {
        recursiveSearch(QDir(path));
        enqueueDone = true;
    });

    connect(dirThread, &QThread::finished, dirThread, &QThread::deleteLater);
    dirThread->start();
    startTasks(QThread::idealThreadCount());
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
            taskQueue.enqueue(entry);
            taskAvailable.release();
        }
    }
}

void ContentSearchWorker::startTasks(int numThreads)
{
    qDebug() << numThreads;
    for (int i = 0; i < numThreads; i++)
    {
        activeThreadCount++;
        QThreadPool::globalInstance()->start(new ContentSearchTask(this));
    }
}
