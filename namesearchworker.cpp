#include "searchworker.h"
#include <QDebug>

SearchWorker::SearchWorker(QObject *parent)
{
    cancelRequested = false;
}

void SearchWorker::startSearch(const QString &path, const QString &searchQuery)
{
    cancelRequested = false;
    recursiveSearch(QDir(path), searchQuery);
    if (cancelRequested){
        emit searchCanceled();
    }
    else
        emit searchFinished();
}

void SearchWorker::cancelSearch()
{
    cancelRequested = true;
}

void SearchWorker::recursiveSearch(const QDir &dir, const QString &searchQuery)
{
    if (cancelRequested)
        return;

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);
    for (QFileInfo &entry : entries)
    {
        if (cancelRequested)
            return;

        if (entry.fileName().contains(searchQuery, Qt::CaseInsensitive))
        {
            emit fileFound(entry);
        }
        if (entry.isDir())
        {
            recursiveSearch(QDir(entry.filePath()), searchQuery);
        }
    }
}
