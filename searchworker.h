#ifndef SEARCHWORKER_H
#define SEARCHWORKER_H

#include <QObject>
#include <QAtomicInt>
#include <QDir>

class SearchWorker : public QObject
{
    Q_OBJECT
public:
    explicit SearchWorker(QObject *parent = nullptr);

signals:
    void fileFound(const QFileInfo &file);
    void searchFinished();
    void searchCanceled();

public slots:
    void startSearch(const QString &path, const QString &searchQuery);
    void cancelSearch();

private:
    void recursiveSearch(const QDir &dir, const QString &searchTerm);
    QAtomicInt cancelRequested;
};

#endif // SEARCHWORKER_H
