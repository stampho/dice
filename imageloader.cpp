#include "imageloader.h"

#include "imageexplorer.h"
#include "API/dimage.h"

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
    m_dimage = new DImage(path.toStdString().c_str());
    Q_EMIT(loaded(m_dimage));
}

void ImageLoader::load(QModelIndex index)
{
    QString path = m_explorer->getSelectedFile(index);
    load(path);
}

DImage* ImageLoader::get() {
    return m_dimage;
}
