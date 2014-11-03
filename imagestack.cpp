#include "imagestack.h"

#include <QDebug>

ImageStack::ImageStack(QObject* parent)
    : QObject(parent)
{
}

ImageStack::ImageStack(cv::Mat image, QObject* parent)
    : QObject(parent)
{
    m_stack[Original] = image;

    connect(this, SIGNAL(preProcessDone(int)), this, SLOT(detectEdges(int)));
    connect(this, SIGNAL(detectEdgesDone(int)), this, SLOT(detectFaces(int)));
    connect(this, SIGNAL(detectFacesDone(int, QVector<Face>)), this, SLOT(detectCubes(int, QVector<Face>)));
    connect(this, SIGNAL(detectCubesDone(int, QVector<Cube>)), this, SLOT(detectTops(int, QVector<Cube>)));
    connect(this, SIGNAL(detectTopsDone(int, QVector<cv::Mat>)), this, SLOT(detectPips(int, QVector<cv::Mat>)));
    connect(this, SIGNAL(detectPipsDone(int, int)), this, SIGNAL(ready(int, int)));
}

ImageStack::~ImageStack()
{
}

void ImageStack::init()
{
    m_thresholdParams.thresh = 127;
    m_thresholdParams.maxval = 255;
    m_thresholdParams.type = cv::THRESH_BINARY;
    preProcess();
}

cv::Mat ImageStack::getImage(Phase phase)
{
    return m_stack[phase];
}

ThresholdParams* ImageStack::getThresholdParams()
{
    return &m_thresholdParams;
}

void ImageStack::preProcess()
{
    cv::Mat image = m_stack[Original].clone();
    if (image.channels() > 1)
        cv::cvtColor(image, image, CV_RGB2GRAY);
    //cv::equalizeHist(image, image);

    m_stack[PreProcess] = image;
    Q_EMIT(preProcessDone(PreProcess));
}

void ImageStack::detectEdges(int prev)
{
    Q_UNUSED(prev);

    cv::Mat image = m_stack[PreProcess].clone();
    cv::threshold(image, image, m_thresholdParams.thresh, m_thresholdParams.maxval, m_thresholdParams.type);

    m_stack[EdgeDetection] = image;
    Q_EMIT(detectEdgesDone(EdgeDetection));
}

void ImageStack::detectFaces(int prev)
{
    Q_UNUSED(prev);

    cv::Mat image = m_stack[PreProcess].clone();
    cv::Mat imageBin = m_stack[EdgeDetection];

    QVector<Face> faces = ImageStack::collectFaces(imageBin);
    for (int i = 0; i < faces.size(); ++i) {
        Face face = faces.at(i);
        image = face.draw(image);
    }

    m_stack[FaceDetection] = image;
    Q_EMIT(detectFacesDone(FaceDetection, faces));
}

void ImageStack::detectCubes(int prev, QVector<Face> faces)
{
    Q_UNUSED(prev);
    cv::Mat image = m_stack[PreProcess].clone();

    QVector<Cube> cubes = QVector<Cube>::fromStdVector(Cube::collectCubes(faces.toStdVector()));
    for (int i = 0; i < cubes.size(); ++i) {
        Cube cube = cubes.at(i);
        cv::Scalar color = (i%2) ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 255, 255);
        image = cube.draw(image, color);
    }

    m_stack[CubeDetection] = image;
    Q_EMIT(detectCubesDone(CubeDetection, cubes));
}

void ImageStack::detectTops(int prev, QVector<Cube> cubes)
{
    Q_UNUSED(prev);
    cv::Mat imageBin = m_stack[EdgeDetection];
    cv::Mat image = cv::Mat::zeros(imageBin.rows, imageBin.cols, CV_8UC1);
    image.setTo(cv::Scalar(255));

    QVector<cv::Mat> topFaces;
    for (int i = 0; i < cubes.size(); ++i) {
        Cube cube = cubes.at(i);
        Face topFace = cube.getTopFace();

        cv::Mat croppedFace = topFace.crop(imageBin);
        topFaces.append(croppedFace);

        cv::bitwise_xor(image, croppedFace, image);
    }

    m_stack[TopDetection] = image;
    Q_EMIT(detectTopsDone(TopDetection, topFaces));
}

void ImageStack::detectPips(int prev, QVector<cv::Mat> topFaces)
{
    Q_UNUSED(prev);
    cv::Mat image = m_stack[PreProcess].clone();

    QVector<Pip> pips;
    for (int i = 0; i < topFaces.size(); ++i) {
        cv::Mat topFace = topFaces.at(i);
        QVector<Pip> pipsOfTopFace = ImageStack::collectPips(topFace);
        pips += pipsOfTopFace;
    }

    m_stack[PipDetection] = ImageStack::drawPips(image, pips);
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
    else if (id.compare("threshTypeGroup") == 0) {
        m_thresholdParams.type = value;
    }

    detectEdges(PreProcess);
}


using namespace cv;

QVector<Face> ImageStack::collectFaces(Mat imageBin)
{
    QVector<Face> faces;

    // Find contours
    std::vector<Contour> contours;
    findContours(imageBin.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    std::vector<Point> approx;
    for (size_t i = 0; i < contours.size(); ++i) {
        approxPolyDP(
                    Mat(contours[i]),
                    approx,
                    arcLength(Mat(contours[i]), true) * 0.02,
                    true
        );

        if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
            continue;

        if (approx.size() == 4) {
            // Do not store the border of the image
            if (approx[0].x == 1 && approx[0].y == 1 &&
                    approx[2].x >= imageBin.cols-5 && approx[2].y >= imageBin.rows-5) {
                continue;
            }
            faces.append(Face(approx));
        }
    }

    return faces;
}

QVector<Pip> ImageStack::collectPips(Mat imageBin)
{
    QVector<Pip> pips;

    // Find contours
    std::vector<Contour> contours;
    findContours(imageBin.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    std::vector<Point> approx;
    for (size_t i = 0; i < contours.size(); ++i) {
        approxPolyDP(
                Mat(contours[i]),
                approx,
                arcLength(Mat(contours[i]), true) * 0.02,
                true
        );

        if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
            continue;

        if (approx.size() >= 8)
            pips.append(approx);
    }

    return pips;

}

Mat ImageStack::drawPips(Mat image, QVector<Pip> pips, Scalar color)
{
    cvtColor(image, image, CV_GRAY2RGB);
    drawContours(image, pips.toStdVector(), -1, color, 2);
    return image;
}
