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
        if (!worker->taskAvailable.tryAcquire(100)) { // timeout to check cancel
            if (worker->cancelRequested || (worker->enqueueDone && worker->taskQueue.empty()))
                break;
            continue;
        }

        QFileInfo file;
        if (!worker->taskQueue.dequeue(file))
            continue; 

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

    int remaining = --worker->activeThreadCount;
    if (remaining == 0) {
        if (worker->cancelRequested)
            emit worker->searchCanceled();
        else
            emit worker->searchFinished();
    }
}
