#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 3);

    // Navigation Bar
    dirModel = new QFileSystemModel(this);
    dirModel->setRootPath(QDir::rootPath());
    dirModel->setFilter(QDir::AllDirs | QDir::Hidden | QDir::NoDotAndDotDot);

    ui->navigationTree->setModel(dirModel);
    ui->navigationTree->hideColumn(1);
    ui->navigationTree->hideColumn(2);
    ui->navigationTree->hideColumn(3);

    // Table View
    fileModel = new FileSystemModel(this);
    fileModel->setCurrentPath(QDir::homePath());

    ui->fileTable->setModel(fileModel);
    fileModel->loadDirectory(fileModel->getCurrentPath());

    ui->fileTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->fileTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->fileTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Interaction
    connect(ui->goUpDirButton, &QPushButton::clicked, this, &MainWindow::onUpDirButtonClicked);
    connect(ui->navigationTree, &QTreeView::clicked, this, &MainWindow::onTreeItemSelected);
    connect(ui->fileTable, &QTableView::doubleClicked, this, &MainWindow::onFileSystemItemSelected);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QVector<QFileInfo> MainWindow::searchFile(const QString &directory, const QString &searchName)
{
    QVector<QFileInfo> result;
    QDir dir(directory);
    dir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);

    QFileInfoList entries = dir.entryInfoList();

    for (auto entry : entries)
    {
        if (entry.fileName().contains(searchName, Qt::CaseInsensitive))
        {
            result.append(entry);
        }
        if (entry.isDir())
        {
            result += searchFile(entry.absoluteFilePath(), searchName);
        }
    }

    return result;
}

void MainWindow::onFileSystemItemSelected(const QModelIndex &index)
{
    QString path = fileModel->filePath(index.row());
    QFileInfo info(path);
    if (info.isDir())
    {
        fileModel->loadDirectory(path);
    }
    else
    {
        QMessageBox::information(this, "File Clicked", "Selected file: " + path);
    }
}

void MainWindow::onTreeItemSelected(const QModelIndex &index)
{
    QString path = dirModel->filePath(index);
    QFileInfo info(path);
    if (info.isDir())
    {
        fileModel->loadDirectory(path);
    }
}

void MainWindow::onUpDirButtonClicked()
{
    QDir dir(fileModel->getCurrentPath());
    if (dir.cdUp())
    {
        fileModel->loadDirectory(dir.absolutePath());
    }
}

void MainWindow::onSearchButtonClicked()
{
    QString searchName = ui->pathInput->text().trimmed();
    if (searchName.isEmpty())
        return;
    QString directory = fileModel->getCurrentPath();
    QVector<QFileInfo> result = searchFile(directory, searchName);
    fileModel->displaySearchResult(result);
}
