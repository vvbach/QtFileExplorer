#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include <QObject>
#include <QFileInfo>
#include "abstractsearchworker.h"
#include <QThread>

enum class SearchMode { Name, Content };

class SearchController : public QObject
{
    Q_OBJECT
public:
    explicit SearchController(QObject *parent = nullptr);
    ~SearchController();

    void startSearch(const QString &dir, const QString &query);
    void setWorker(QString& mode);
    void cancel();

signals:
    void fileFound(const QFileInfo &info);
    void searchFinished();
    void searchCanceled();

private:
    QThread* workerThread;
    AbstractSearchWorker* worker;
    bool isSearching = false;
};

#endif // SEARCHCONTROLLER_H
