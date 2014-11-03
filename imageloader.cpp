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
{
}

void ImageLoader::load(QString path)
{
    m_matImage = cv::imread(path.toStdString().c_str(), CV_LOAD_IMAGE_COLOR);
    Q_EMIT(loaded(m_matImage));
}

void ImageLoader::load(QModelIndex index)
{
    QString path = m_explorer->getSelectedFile(index);
    load(path);
}

cv::Mat ImageLoader::get() {
    return m_matImage;
}
