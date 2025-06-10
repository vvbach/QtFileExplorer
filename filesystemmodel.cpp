#include "filesystemmodel.h"
#include <QApplication>
#include <QStyle>
#include <QDebug>

FileSystemModel::FileSystemModel(QObject *parent) : QAbstractTableModel(parent) {}

int FileSystemModel::rowCount(const QModelIndex &) const
{
    return fileInfoList.size();
}

int FileSystemModel::columnCount(const QModelIndex &) const
{
    return searchMode ? 6 : 5; // Name, Size, Created, Modified, Permissions
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= fileInfoList.size())
        return QVariant();

    const QFileInfo &info = fileInfoList.at(index.row());

    QFile::Permissions permissisons = info.permissions();
    QString permStr;
    permStr += (permissisons & QFile::ReadOwner) ? "r" : "-";
    permStr += (permissisons & QFile::WriteOwner) ? "w" : "-";
    permStr += (permissisons & QFile::ExeOwner) ? "x" : "-";
    permStr += (permissisons & QFile::ReadGroup) ? "r" : "-";
    permStr += (permissisons & QFile::WriteGroup) ? "w" : "-";
    permStr += (permissisons & QFile::ExeGroup) ? "x" : "-";
    permStr += (permissisons & QFile::ReadOther) ? "r" : "-";
    permStr += (permissisons & QFile::WriteOther) ? "w" : "-";
    permStr += (permissisons & QFile::ExeOther) ? "x" : "-";

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:
            return info.fileName();
        case 1:
            return info.isDir() ? "<DIR>" : QString::number(info.size());
        case 2:
            return info.birthTime().toString("yyyy-MM-dd HH:mm:ss");
        case 3:
            return info.lastModified().toString("yyyy-MM-dd HH:mm:ss");
        case 4:
            return permStr;
        case 5:
            return info.absoluteFilePath();
        }
    }

    if (role == Qt::DecorationRole && index.column() == 0)
    {
        QStyle *style = QApplication::style();
        QIcon icon = info.isDir() ? style->standardIcon(QStyle::SP_DirIcon)
                                  : style->standardIcon(QStyle::SP_FileIcon);
        return icon;
    }

    return QVariant();
}

QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return "Name";
        case 1:
            return "Size";
        case 2:
            return "Created";
        case 3:
            return "Modified";
        case 4:
            return "Permission";
        case 5:
            return "Path";
        }
    }
    return QVariant();
}

void FileSystemModel::loadDirectory(const QString &path)
{
    QDir dir(path);
    fileInfoList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries, QDir::DirsFirst | QDir::Name);
    currentPath = path;

    searchMode = false;

    beginResetModel();
    endResetModel();
}

QString FileSystemModel::filePath(int row) const
{
    if (row >= 0 && row < fileInfoList.size())
    {
        return fileInfoList[row].absoluteFilePath();
    }
    return QString();
}

QString FileSystemModel::getCurrentPath()
{
    return currentPath;
}

void FileSystemModel::setCurrentPath(const QString &path)
{
    currentPath = path;
}

void FileSystemModel::displaySearchResult(const QFileInfo &file)
{
    beginInsertRows(QModelIndex(), fileInfoList.size(), fileInfoList.size());
    fileInfoList.append(file);
    endInsertRows();
}

void FileSystemModel::clear()
{
    beginResetModel();
    fileInfoList.clear();
    searchMode = true;
    endResetModel();
}