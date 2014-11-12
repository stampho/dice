#include "imageloader.h"

#include "imageexplorer.h"

#include <opencv2/highgui/highgui.hpp>

#include <QDebug>

ImageLoader::ImageLoader(QObject *parent) :
    QObject(parent)
{
}

ImageLoader::ImageLoader(ImageExplorer* explorer, QObject* parent)
    : QObject(parent)
    , m_explorer(explorer)
    , m_info(new ImageInfo())
{
}

void ImageLoader::load(QString path)
{
    m_matImage = cv::imread(path.toStdString().c_str(), CV_LOAD_IMAGE_COLOR);
    updateInfo(path);
    Q_EMIT(loaded(m_matImage));
}

void ImageLoader::load(QModelIndex index)
{
    QString path = m_explorer->getSelectedFile(index);
    load(path);
}

cv::Mat ImageLoader::get()
{
    return m_matImage;
}

ImageInfo* ImageLoader::getInfo()
{
    return m_info;
}

void ImageLoader::updateInfo(QString path)
{
    QString depth;
    switch(m_matImage.depth()) {
        case CV_8U:
            depth = QString("8-bit unsigned integer");
            break;
        case CV_8S:
            depth = QString("8-bit integer");
            break;
        case CV_16U:
            depth = QString("16-bit unsigned integer");
            break;
        case CV_16S:
            depth = QString("16-bit integer");
            break;
        case CV_32S:
            depth = QString("32-bit integer");
            break;
        case CV_32F:
            depth = QString("32-bit float");
            break;
        case CV_64F:
            depth = QString("64-bit float");
            break;
        default:
            depth = QString::number(m_matImage.depth());
    }

    m_info->path = path;
    m_info->cols = m_matImage.cols;
    m_info->rows = m_matImage.rows;
    m_info->channels = m_matImage.channels();
    m_info->depth = depth;
}
