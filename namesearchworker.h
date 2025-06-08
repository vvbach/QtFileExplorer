#ifndef NAMESEARCHWORKER_H
#define NAMESEARCHWORKER_H

#include <QObject>
#include <QAtomicInt>
#include <QDir>
#include "abstractsearchworker.h"

class NameSearchWorker : public AbstractSearchWorker
{
    Q_OBJECT
public:
    NameSearchWorker();

public slots:
    void startSearch(const QString &path, const QString &searchQuery) override;
    void cancelSearch() override;

private:
    void recursiveSearch(const QDir &dir, const QString &searchTerm);
    QAtomicInt cancelRequested;
};

#endif // NAMESEARCHWORKER_H
