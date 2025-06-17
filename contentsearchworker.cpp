#include "contentsearchworker.h"
#include <QThreadPool>
#include <QDir>
#include <QDebug>

ContentSearchWorker::ContentSearchWorker() {}

void ContentSearchWorker::cancelSearch()
{
    cancelRequested = true;
    while (activeThreadCount > 0) {
        QThread::msleep(10);
    }

    taskQueue.clear();
}

void ContentSearchWorker::startSearch(const QString &path, const QString &searchQuery)
{
    cancelRequested = false;
    enqueueDone = false;
    this->searchQuery = searchQuery;
    startTasks(QThread::idealThreadCount());
    QThread *dirThread = QThread::create([this, path]()
    {
        recursiveSearch(QDir(path));
        enqueueDone = true;
    });

    connect(dirThread, &QThread::finished, dirThread, &QThread::deleteLater);
    dirThread->start();
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
            recursiveSearch(QDir(entry.absoluteFilePath()));
        }
        else
        {
            taskQueue.enqueue(entry.absoluteFilePath());
        }
    }
}

void ContentSearchWorker::startTasks(int numThreads)
{
    qDebug() << numThreads;
    for (int i = 0; i < numThreads; i++)
    {
        activeThreadCount++;
        QThreadPool::globalInstance()->start(new ContentSearchTask(this, i));
    }
}
