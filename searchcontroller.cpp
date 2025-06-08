#include "searchcontroller.h"
#include "namesearchworker.h"
#include "contentsearchworker.h"

SearchController::SearchController(QObject *parent) : QObject(parent) {
    worker = nullptr;
    workerThread = nullptr;
}

SearchController::~SearchController() {
    cancel();
}

void SearchController::startSearch(const QString &path, const QString &query) {
    if (!worker) return;

    qDebug() << "Start search";
    workerThread = new QThread();

    worker->moveToThread(workerThread);

    connect(workerThread, &QThread::started, [=]() {
        worker->startSearch(path, query);
    });

    connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);

    workerThread->start();
}

void SearchController::setWorker(QString& mode)
{
    if (worker) {
        worker->deleteLater();  
        worker = nullptr;
    }

    if (mode == "Name"){
        worker = new NameSearchWorker();
    } else if (mode == "Content"){
        worker = new ContentSearchWorker();
    } else {
        worker = nullptr;
        return;
    }

    connect(worker, &AbstractSearchWorker::fileFound, this, &SearchController::fileFound);
    connect(worker, &AbstractSearchWorker::searchFinished, this, &SearchController::searchFinished);
    connect(worker, &AbstractSearchWorker::searchCanceled, this, &SearchController::searchCanceled);
}

void SearchController::cancel(){
    if (worker){
        worker->cancelSearch();
    }
    if (workerThread){
        workerThread->quit();
        workerThread->wait();
        workerThread = nullptr;
        worker = nullptr;
    }
}