#include "dicewindow.h"
#include "ui_dicewindow.h"
#include "imageexplorer.h"
#include "imageloader.h"
#include "imagestack.h"

#include <QDebug>

#include "API/dimage.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/*
 * Based on: http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap
 */
static QImage Mat2QImage(const cv::Mat& inMat)
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


DiceWindow::DiceWindow(QWidget* parent) :
    QMainWindow(parent),
    m_ui(new Ui::DiceWindow)
{
    m_ui->setupUi(this);

    for (int i = 0; i < ImageStack::PhaseCount; ++i) {
        m_images.append(new QLabel());
        m_ui->imageSlide->layout()->addWidget(m_images.at(i));
    }

    ImageExplorer* imageExplorer = new ImageExplorer(m_ui);
    ImageLoader* imageLoader = new ImageLoader(imageExplorer);

    connect(m_ui->dirBrowser, SIGNAL(clicked(QModelIndex)), imageExplorer, SLOT(dirSelected(QModelIndex)));
    connect(m_ui->fileBrowser, SIGNAL(clicked(QModelIndex)), imageLoader, SLOT(load(QModelIndex)));

    connect(imageLoader, SIGNAL(loaded(DImage*)), this, SLOT(initImageStack(DImage*)));
}

DiceWindow::~DiceWindow()
{
    delete m_ui;
}

void DiceWindow::initImageStack(DImage* dimage) {
    m_imageStack = new ImageStack(dimage);
    connect(m_imageStack, SIGNAL(ready(int, int)), this, SLOT(showImageStack(int, int)));
    // TODO: implement proper init function in ImageStack instead of using this
    m_imageStack->preProcess();
}

void DiceWindow::showImageStack(int phase, int result)
{
    Q_UNUSED(phase);
    Q_UNUSED(result);

    for (int i = 0; i < m_images.size(); ++i) {
        QImage qimage = Mat2QImage(m_imageStack->getImage((ImageStack::Phase)i)->getMat());
        QPixmap pixmap = QPixmap::fromImage(qimage);
        QLabel* label = m_images.at(i);
        label->setPixmap(pixmap);
        label->setFixedSize(pixmap.size());
    }
}
