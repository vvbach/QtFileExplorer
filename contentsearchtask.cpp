#include "contentsearchworker.h"

ContentSearchTask::ContentSearchTask(ContentSearchWorker *worker, int number)
    : worker(worker), number(number)
{
    setAutoDelete(true);
}

void ContentSearchTask::run()
{
    while (!worker->cancelRequested)
    {
        QString filePath;
        bool gotTask = false;

        while (!(gotTask = worker->taskQueue.dequeue(filePath)) &&
               !worker->enqueueDone.load() &&
               !worker->cancelRequested.load()) {
            qDebug() << "Thread " << number << "is waiting";
            QThread::msleep(10);
        }

        if (!gotTask) {
            if (worker->cancelRequested || worker->enqueueDone.load())
                break;
            continue;
        }

        QFile f(filePath);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&f);
            while (!in.atEnd() && !worker->cancelRequested)
            {
                QString line = in.readLine();
                if (line.contains(worker->searchQuery, Qt::CaseInsensitive))
                {
                    QFileInfo file(filePath);
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
        qDebug() << "Done";
    }
}
