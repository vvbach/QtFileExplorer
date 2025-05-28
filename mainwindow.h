#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
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

private slots:
    void onFileSystemItemSelected(const QModelIndex &index);
    void onTreeItemSelected(const QModelIndex &index);
    void onPressingUpDirButton();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *dirModel;
    FileSystemModel *fileModel;
};
#endif 
