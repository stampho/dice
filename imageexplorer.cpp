#include "imageexplorer.h"
#include "ui_dicewindow.h"

#include <QDebug>
#include <QDir>

ImageExplorer::ImageExplorer(QObject* parent)
    : QObject(parent)
{}

ImageExplorer::ImageExplorer(Ui::DiceWindow* ui, QObject* parent)
    : QObject(parent)
    , m_ui(ui)
{
    m_imageFilter << "*.jpg" << "*.png";

    initDirModel();
    initFileModel();
}

void ImageExplorer::initDirModel()
{
    m_dirModel = new QFileSystemModel(this);
    m_dirModel->setRootPath("");
    m_dirModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    m_ui->dirBrowser->setModel(m_dirModel);
    m_ui->dirBrowser->setColumnHidden(1, true); // Size
    m_ui->dirBrowser->setColumnHidden(2, true); // Type
    m_ui->dirBrowser->setColumnHidden(3, true); // Date
    m_ui->dirBrowser->setRootIndex(m_dirModel->setRootPath("/"));
    m_ui->dirBrowser->setCurrentIndex(m_dirModel->index(QDir::currentPath()));
}

void ImageExplorer::initFileModel()
{
    m_fileModel = new QFileSystemModel(this);
    m_fileModel->setRootPath("");
    m_fileModel->setNameFilterDisables(false);
    m_ui->fileBrowser->setModel(m_fileModel);
    dirSelected(m_ui->dirBrowser->currentIndex());
}

void ImageExplorer::dirSelected(QModelIndex index)
{
    QString path = m_dirModel->fileInfo(index).absoluteFilePath();
    m_ui->fileBrowser->setRootIndex(m_fileModel->setRootPath(path));
    m_fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    m_fileModel->setNameFilters(m_imageFilter);
}

QString ImageExplorer::getSelectedFile(QModelIndex index)
{
    return m_fileModel->fileInfo(index).absoluteFilePath();
}
