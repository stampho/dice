#include "imageviewer.h"
#include "ui_dicewindow.h"
#include "imageexplorer.h"

#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "API/dimage.h"

ImageViewer::ImageViewer(QObject* parent)
    : QObject(parent)
{}

ImageViewer::ImageViewer(Ui::DiceWindow* ui, ImageExplorer* explorer, QObject* parent)
    : QObject(parent)
    , m_ui(ui)
    , m_explorer(explorer)
{
}

void ImageViewer::load(QString path)
{
    DImage dimage(path.toStdString().c_str());
    QImage qimage = Mat2QImage(dimage.getMat());
    QPixmap pixmap = QPixmap::fromImage(qimage);
    m_ui->mainImage->setPixmap(pixmap);
    m_ui->mainImage->setFixedSize(pixmap.size());
}

void ImageViewer::load(QModelIndex index)
{
    QString path = m_explorer->getSelectedFile(index);
    load(path);
}

/*
 * Based on: http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap
 */
QImage ImageViewer::Mat2QImage(const cv::Mat& inMat)
{
    switch (inMat.type()) {
        case CV_8UC4:
        {
            QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32);
            return image;
        }

        case CV_8UC3:
        {
            QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888);
            return image.rgbSwapped();
        }

        case CV_8UC1:
        {
            static QVector<QRgb> sColorTable;

            // only create our color table once
            if (sColorTable.isEmpty()) {
                for (int i = 0; i < 256; ++i)
                    sColorTable.push_back(qRgb(i, i, i));
            }

            QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8);
            image.setColorTable(sColorTable);
            return image;
        }

        default:
            qWarning() << "cv::Mat image type not handled in switch:" << inMat.type();
            break;
    }

    return QImage();
}
