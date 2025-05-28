#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QVector>
#include <QFileInfo>
#include "filesystemmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QVector<QFileInfo> searchFile(const QString& directory, const QString& searchName);

private slots:
    void onFileSystemItemSelected(const QModelIndex &index);
    void onTreeItemSelected(const QModelIndex &index);
    void onUpDirButtonClicked();
    void onSearchButtonClicked();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *dirModel;
    FileSystemModel *fileModel;
};
#endif 
