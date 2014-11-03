#include "dicewindow.h"
#include "ui_dicewindow.h"
#include "imageexplorer.h"
#include "imageloader.h"
#include "imagestack.h"

#include <QDebug>

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


DiceWindow::DiceWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::DiceWindow)
    , m_imageStack(NULL)
{
    m_ui->setupUi(this);

    for (int i = 0; i < ImageStack::PhaseCount; ++i) {
        m_images.append(new QLabel());
        m_ui->imageSlide->layout()->addWidget(m_images.at(i));
    }

    m_ui->threshTypeGroup->setId(m_ui->binaryButton, cv::THRESH_BINARY);
    m_ui->threshTypeGroup->setId(m_ui->binaryinvButton, cv::THRESH_BINARY_INV);
    m_ui->threshTypeGroup->setId(m_ui->truncButton, cv::THRESH_TRUNC);
    m_ui->threshTypeGroup->setId(m_ui->tozeroButton, cv::THRESH_TOZERO);
    m_ui->threshTypeGroup->setId(m_ui->tozeroinvButton, cv::THRESH_TOZERO_INV);

    ImageExplorer* imageExplorer = new ImageExplorer(m_ui);
    ImageLoader* imageLoader = new ImageLoader(imageExplorer);

    connect(m_ui->dirBrowser, SIGNAL(clicked(QModelIndex)), imageExplorer, SLOT(dirSelected(QModelIndex)));
    connect(m_ui->fileBrowser, SIGNAL(clicked(QModelIndex)), imageLoader, SLOT(load(QModelIndex)));

    connect(imageLoader, SIGNAL(loaded(cv::Mat)), this, SLOT(initImageStack(cv::Mat)));
}

DiceWindow::~DiceWindow()
{
    delete m_ui;
}


void DiceWindow::initImageStack(cv::Mat matImage) {
    if (m_imageStack != NULL)
        delete m_imageStack;

    m_imageStack = new ImageStack(matImage);

    connect(m_imageStack, SIGNAL(ready(int, int)), this, SLOT(showImageStack(int, int)));
    connect(m_imageStack, SIGNAL(ready(int, int)), this, SLOT(initThresh(int, int)));

    connect(m_ui->threshSlider, SIGNAL(valueChanged(int)), m_imageStack, SLOT(onThresholdParamChanged(int)));
    connect(m_ui->threshSlider, SIGNAL(valueChanged(int)), this, SLOT(onThreshChanged(int)));
    connect(m_ui->maxvalSlider, SIGNAL(valueChanged(int)), m_imageStack, SLOT(onThresholdParamChanged(int)));
    connect(m_ui->maxvalSlider, SIGNAL(valueChanged(int)), this, SLOT(onThreshChanged(int)));
    connect(m_ui->threshTypeGroup, SIGNAL(buttonPressed(int)), m_imageStack, SLOT(onThresholdParamChanged(int)));

    m_imageStack->init();
}

void DiceWindow::showImageStack(int phase, int result)
{
    Q_UNUSED(phase);
    Q_UNUSED(result);

    for (int i = 0; i < m_images.size(); ++i) {
        QImage qimage = Mat2QImage(m_imageStack->getImage((ImageStack::Phase)i));
        QPixmap pixmap = QPixmap::fromImage(qimage);
        QLabel* label = m_images.at(i);
        label->setPixmap(pixmap);
        label->setFixedSize(pixmap.size());
    }
}

void DiceWindow::initThresh(int phase, int result)
{
    Q_UNUSED(phase);
    Q_UNUSED(result);

    ThresholdParams* params = m_imageStack->getThresholdParams();

    m_ui->threshDisplay->setText(QString::number(params->thresh));
    m_ui->maxvalDisplay->setText(QString::number(params->maxval));

    m_ui->threshSlider->setSliderPosition(params->thresh);
    m_ui->maxvalSlider->setSliderPosition(params->maxval);

    m_ui->threshTypeGroup->button(params->type)->setChecked(true);
}

void DiceWindow::onThreshChanged(int value)
{
    QObject* sender = QObject::sender();
    QString id = sender->objectName();

    if (id.compare("threshSlider") == 0)
        m_ui->threshDisplay->setText(QString::number(value));
    else if (id.compare("maxvalSlider") == 0)
        m_ui->maxvalDisplay->setText(QString::number(value));
}
