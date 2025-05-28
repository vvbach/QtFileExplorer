#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QAbstractTableModel>
#include <QFileInfo>
#include <QDir>
#include <QIcon>

class FileSystemModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FileSystemModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void loadDirectory(const QString &path);
    QString filePath(int row) const;
    
    QString getCurrentPath();
    void setCurrentPath(const QString &path);

private:
    QString currentPath;
    QFileInfoList fileInfoList;
};

#endif // FILESYSTEMMODEL_H
