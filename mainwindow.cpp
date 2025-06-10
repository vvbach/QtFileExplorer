#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>

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
    ui->pathLabel->setText(fileModel->getCurrentPath());

    ui->fileTable->setModel(fileModel);
    fileModel->loadDirectory(fileModel->getCurrentPath());

    ui->fileTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->fileTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->fileTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->fileTable->setContextMenuPolicy(Qt::CustomContextMenu);

    // Interaction
    connect(ui->goUpDirButton, &QPushButton::clicked, this, &MainWindow::onUpDirButtonClicked);
    connect(ui->navigationTree, &QTreeView::clicked, this, &MainWindow::onTreeItemSelected);
    connect(ui->fileTable, &QTableView::doubleClicked, this, &MainWindow::onFileSystemItemSelected);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelButtonClicked);
    connect(ui->fileTable, &QWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

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
        ui->pathLabel->setText(fileModel->getCurrentPath());
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
    if (inSearchMode) return;

    QDir dir(fileModel->getCurrentPath());
    if (dir.cdUp())
    {
        fileModel->loadDirectory(dir.absolutePath());
        ui->pathLabel->setText(fileModel->getCurrentPath());
    }
}

void MainWindow::onSearchButtonClicked()
{
    QString searchQuery = ui->searchInput->text().trimmed();
    if (searchQuery.isEmpty())
        return;
    QString directory = fileModel->getCurrentPath();
    inSearchMode = true;

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
        inSearchMode = false;
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
        qDebug() << "Cancelling search";
        searchController->cancel();
        searchController->deleteLater();
        searchController = nullptr;
    }
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    if (fileModel->searchMode) return;

    QModelIndex index = ui->fileTable->indexAt(pos);
    QMenu contextMenu(this);

    QAction *newFileAction = contextMenu.addAction("New File");
    QAction *newDirAction = contextMenu.addAction("New Directory");
    QAction *renameAction = nullptr;
    QAction *deleteAction = nullptr;

    if (index.isValid()) {
        renameAction = contextMenu.addAction("Rename");
        deleteAction = contextMenu.addAction("Delete");
    }

    QAction *selectedAction = contextMenu.exec(ui->fileTable->viewport()->mapToGlobal(pos));
    QString currentPath = fileModel->getCurrentPath();

    if (selectedAction == newFileAction) {
        QString fileName = QInputDialog::getText(this, "New File", "Enter file name:");
        if (!fileName.isEmpty()) {
            QFile file(currentPath + "/" + fileName);
            file.open(QIODevice::WriteOnly);
            file.close();
            fileModel->loadDirectory(currentPath);
        }
    }
    else if (selectedAction == newDirAction) {
        QString dirName = QInputDialog::getText(this, "New Directory", "Enter directory name:");
        if (!dirName.isEmpty()) {
            QDir(currentPath).mkdir(dirName);
            fileModel->loadDirectory(currentPath);
        }
    }
    else if (selectedAction == renameAction) {
        QString filePath = fileModel->filePath(index.row());
        QFileInfo fileInfo(filePath);
        QString newName = QInputDialog::getText(this, "Rename", "Enter new name:", QLineEdit::Normal, fileInfo.fileName());
        if (!newName.isEmpty() && newName != fileInfo.fileName()) {
            QFile::rename(fileInfo.absoluteFilePath(), fileInfo.absolutePath() + "/" + newName);
            fileModel->loadDirectory(currentPath);
        }
    }
    else if (selectedAction == deleteAction) {
        QString filePath = fileModel->filePath(index.row());
        QFileInfo fileInfo(filePath);
        QString path = fileInfo.absoluteFilePath();
        bool success = fileInfo.isDir() ? QDir(path).removeRecursively() : QFile::remove(path);
        if (!success) {
            QMessageBox::warning(this, "Delete", "Failed to delete file/directory.");
        }
        fileModel->loadDirectory(currentPath);
    }
}
