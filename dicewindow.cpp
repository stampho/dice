#include "dicewindow.h"
#include "ui_dicewindow.h"

#include <QDebug>
#include <QDir>
#include <QFileSystemModel>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>

#include "API/dimage.h"

// Based on: http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap
QImage Mat2QImage(const cv::Mat& inMat)
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
                    sColorTable.push_back( qRgb( i, i, i ) );
            }

            QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8);
            image.setColorTable( sColorTable );
            return image;
        }

        default:
            qWarning() << "cv::Mat image type not handled in switch:" << inMat.type();
            break;
    }

    return QImage();
}

DiceWindow::DiceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DiceWindow)
{
    ui->setupUi(this);

    QFileSystemModel* dirModel = new QFileSystemModel(this);
    dirModel->setRootPath("");
    dirModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    ui->treeView->setModel(dirModel);

    DImage dimage("../dice/samples/dice-red.png");
    QImage qimage = Mat2QImage(dimage.getMat());
    QPixmap pixmap = QPixmap::fromImage(qimage);
    ui->image->setPixmap(pixmap);
    ui->image->setFixedSize(pixmap.size());
}

DiceWindow::~DiceWindow()
{
    delete ui;
}
