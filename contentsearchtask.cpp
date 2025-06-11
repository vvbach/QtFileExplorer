#include "contentsearchworker.h"

ContentSearchTask::ContentSearchTask(ContentSearchWorker *worker)
    : worker(worker)
{
    setAutoDelete(true);
}

void ContentSearchTask::run()
{
    while (!worker->cancelRequested)
    {
        QFileInfo file;
        {
            QMutexLocker locker(&worker->queueMutex);
            while (worker->taskQueue.isEmpty() && !worker->enqueueDone && !worker->cancelRequested)
            {
                qDebug() << "waiting task";
                worker->queueNotEmpty.wait(&worker->queueMutex);
            }
            if (worker->cancelRequested || (worker->taskQueue.isEmpty() && worker->enqueueDone))
            {
                qDebug() << "stop task";
                break;
            }
            file = worker->taskQueue.dequeue();
        }
        QFile f(file.absoluteFilePath());
        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&f);
            while (!in.atEnd() && !worker->cancelRequested)
            {
                QString line = in.readLine();
                if (line.contains(worker->searchQuery, Qt::CaseInsensitive))
                {
                    emit worker->fileFound(file);
                    break;
                }
            }
            f.close();
        }
    }

    bool lastThread = false;
    {
        QMutexLocker locker(&worker->queueMutex);
        worker->activeThreadCount--;
        lastThread = (worker->activeThreadCount == 0);
    }

    if (lastThread)
    {
        if (worker->cancelRequested)
            emit worker->searchCanceled();
        else
            emit worker->searchFinished();
    }
}
