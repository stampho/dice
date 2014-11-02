#include "imagestack.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <API/core.h>
#include <API/cube.h>
#include <API/face.h>

#include <QDebug>

ImageStack::ImageStack(QObject* parent)
    : QObject(parent)
{
}

ImageStack::ImageStack(DImage* image, QObject* parent)
    : QObject(parent)
{
    m_stack[Original] = DImage(image->getMat());

    connect(this, SIGNAL(preProcessDone(int)), this, SLOT(detectEdges(int)));
    connect(this, SIGNAL(detectEdgesDone(int)), this, SLOT(detectFaces(int)));
    connect(this, SIGNAL(detectFacesDone(int, QVector<Face>)), this, SLOT(detectCubes(int, QVector<Face>)));
    connect(this, SIGNAL(detectCubesDone(int, QVector<Cube>)), this, SLOT(detectTops(int, QVector<Cube>)));
    connect(this, SIGNAL(detectTopsDone(int, QVector<DImage>)), this, SLOT(detectPips(int, QVector<DImage>)));
    connect(this, SIGNAL(detectPipsDone(int, int)), this, SIGNAL(ready(int, int)));
}

ImageStack::~ImageStack()
{
    //qDebug("destruct ImageStack");
}

void ImageStack::init()
{
    m_thresholdParams.thresh = 127;
    m_thresholdParams.maxval = 255;
    m_thresholdParams.invert = false;
    preProcess();
}

DImage* ImageStack::getImage(Phase phase)
{
    return &m_stack[phase];
}

void ImageStack::preProcess()
{
    DImage* image = new DImage(m_stack[Original].getMat());

    m_stack[PreProcess] = DImage(image->getGray());
    Q_EMIT(preProcessDone(PreProcess));
}

void ImageStack::detectEdges(int prev)
{
    cv::Mat image = m_stack[prev].getMat().clone();
    cv::threshold(image, image, m_thresholdParams.thresh, m_thresholdParams.maxval, m_thresholdParams.invert ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY);

    m_stack[EdgeDetection] = DImage(image);
    Q_EMIT(detectEdgesDone(EdgeDetection));
}

void ImageStack::detectFaces(int prev)
{
    cv::Mat image = m_stack[PreProcess].getMat().clone();
    cv::Mat binImage = m_stack[prev].getMat().clone();

    QVector<Face> faces = QVector<Face>::fromStdVector(DImage(binImage).collectFaces());
    for (int i = 0; i < faces.size(); ++i) {
        Face face = faces.at(i);
        image = face.draw(image);
    }

    m_stack[FaceDetection] = DImage(image);
    Q_EMIT(detectFacesDone(FaceDetection, faces));
}

void ImageStack::detectCubes(int prev, QVector<Face> faces)
{
    Q_UNUSED(prev);
    cv::Mat image = m_stack[PreProcess].getMat();

    QVector<Cube> cubes = QVector<Cube>::fromStdVector(Cube::collectCubes(faces.toStdVector()));
    for (int i = 0; i < cubes.size(); ++i) {
        Cube cube = cubes.at(i);
        cv::Scalar color = (i%2) ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 255, 255);
        image = cube.draw(image, color);
    }

    m_stack[CubeDetection] = DImage(image);
    Q_EMIT(detectCubesDone(CubeDetection, cubes));
}

void ImageStack::detectTops(int prev, QVector<Cube> cubes)
{
    Q_UNUSED(prev);
    cv::Mat gray = m_stack[PreProcess].getMat();
    cv::Mat image = cv::Mat::zeros(gray.rows, gray.cols, CV_8UC1);
    image.setTo(cv::Scalar(255));

    QVector<DImage> topFaces;
    for (int i = 0; i < cubes.size(); ++i) {
        Cube cube = cubes.at(i);
        Face topFace = cube.getTopFace();

        cv::Mat croppedFace = topFace.crop(gray);
        topFaces.append(DImage(croppedFace));

        cv::bitwise_xor(image, croppedFace, image);
    }

    m_stack[TopDetection] = DImage(image);
    Q_EMIT(detectTopsDone(TopDetection, topFaces));
}

void ImageStack::detectPips(int prev, QVector<DImage> topFaces)
{
    Q_UNUSED(prev);
    DImage* image = new DImage(m_stack[PreProcess].getMat());

    QVector<Pip> pips;
    for (int i = 0; i < topFaces.size(); ++i) {
        DImage topFace = topFaces.at(i);
        QVector<Pip> pipsOfTopFace = QVector<Pip>::fromStdVector(topFace.collectPips());
        pips += pipsOfTopFace;
    }

    m_stack[PipDetection] = DImage(image->drawPips(pips.toStdVector()));
    Q_EMIT(detectPipsDone(PipDetection, pips.size()));
}

void ImageStack::onThresholdParamChanged(int value)
{
    QObject* sender = QObject::sender();
    QString id = sender->objectName();

    if (id.compare("threshSlider") == 0)
        m_thresholdParams.thresh = value;
    else if (id.compare("maxvalSlider") == 0)
        m_thresholdParams.maxval = value;
    else if (id.compare("threshInvertToggle") == 0)
        m_thresholdParams.invert = (value > 0);

    detectEdges(PreProcess);
}
