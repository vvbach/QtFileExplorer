#ifndef ABSTRACTSEARCHWORKER_H
#define ABSTRACTSEARCHWORKER_H

#include <QObject>
#include <QFileInfo>

class AbstractSearchWorker : public QObject {
    Q_OBJECT
public slots:
    virtual void startSearch(const QString &dir, const QString &query) = 0;
    virtual void cancelSearch() = 0;

signals:
    void fileFound(const QFileInfo &file);
    void searchFinished();
    void searchCanceled();
};

#endif // ABSTRACTSEARCHWORKER_H
