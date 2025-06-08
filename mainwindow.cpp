#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), searchController(nullptr)
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
    connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelButtonClicked);

    ui->cancelButton->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QString searchQuery = ui->pathInput->text().trimmed();
    if (searchQuery.isEmpty())
        return;
    QString directory = fileModel->getCurrentPath();

    if (searchController) {
        qDebug() << "delete search";
        searchController->cancel();
        searchController->deleteLater();
        searchController = nullptr;
        
        qDebug() << "done delete search";
    }

    searchController = new SearchController();
    QString mode = ui->modeBox->currentText();
    searchController->setWorker(mode);
    ui->cancelButton->show();
    ui->searchButton->hide();
    qDebug() << "set search";

    auto finalize = [this]() {
        ui->cancelButton->hide();
        ui->searchButton->show();
    };
    connect(searchController, &SearchController::fileFound,
            fileModel, &FileSystemModel::displaySearchResult);

    connect(searchController, &SearchController::searchFinished,
            this, [=]()
            { 
                finalize();
                qDebug() << "Search finished"; 
            });

    connect(searchController, &SearchController::searchCanceled,
            this, [=]()
            { 
                finalize();
                qDebug() << "Search canceled"; 
            });
        
    qDebug() << "before start search";
    fileModel->clear();
    searchController->startSearch(directory, searchQuery);
}

void MainWindow::onCancelButtonClicked()
{
    if (searchController)
    {
        searchController->cancel();
        searchController->deleteLater();
        searchController = nullptr;
    }
}
