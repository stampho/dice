#include "imagestack.h"

#include <QDebug>

const char* getType(cv::Mat mat)
{
    switch(mat.type()) {
        case CV_8UC4:
            return "CV_8UC4";
        case CV_8UC3:
            return "CV_8UC3";
        case CV_8UC1:
            return "CV_8UC1";
    }

    return "unknown";
}



ImageStack::ImageStack(QObject* parent)
    : QObject(parent)
{
}

ImageStack::ImageStack(cv::Mat image, QObject* parent)
    : QObject(parent)
{
    m_stack[Original] = image;

    m_commonParams.histogram = false;
    m_commonParams.removepips = true;
    m_commonParams.canny = true;

    m_thresholdParams.thresh = 127;
    m_thresholdParams.maxval = 255;
    m_thresholdParams.type = cv::THRESH_BINARY;

    m_cannyParams.lowThreshold = 0;
    m_cannyParams.ratio = 3;
    m_cannyParams.kernelSize = 3;

    m_edgeParams.dilateSize = 5;
    m_edgeParams.dilateType = cv::MORPH_RECT;
    m_edgeParams.dilateBlur = 0;
    m_edgeParams.erodeSize = 5;
    m_edgeParams.erodeType = cv::MORPH_RECT;
    m_edgeParams.erodeBlur = 3;
    m_edgeParams.thresholdType = cv::THRESH_BINARY;

    connect(this, SIGNAL(preProcessDone()), this, SLOT(detectEdges()));
    connect(this, SIGNAL(detectEdgesDone()), this, SLOT(removePips()));
    connect(this, SIGNAL(removePipsDone()), this, SLOT(enhanceEdges()));
    connect(this, SIGNAL(enhanceEdgesDone()), this, SLOT(detectContours()));
    connect(this, SIGNAL(detectContoursDone(QVector<Outline>)), this, SLOT(detectFaces(QVector<Outline>)));
    connect(this, SIGNAL(detectFacesDone(QVector<Face>, bool)), this, SLOT(detectCubes(QVector<Face>, bool)));
    connect(this, SIGNAL(detectCubesDone(QVector<Cube>)), this, SLOT(detectTops(QVector<Cube>)));
    connect(this, SIGNAL(detectTopsDone(QVector<cv::Mat>)), this, SLOT(detectPips(QVector<cv::Mat>)));
    connect(this, SIGNAL(detectPipsDone(int)), this, SIGNAL(ready(int)));
}

ImageStack::~ImageStack()
{
}

cv::Mat ImageStack::getImage(Phase phase)
{
    return m_stack[phase];
}

CommonParams* ImageStack::getCommonParams()
{
    return &m_commonParams;
}

ThresholdParams* ImageStack::getThresholdParams()
{
    return &m_thresholdParams;
}

CannyParams* ImageStack::getCannyParams()
{
    return &m_cannyParams;
}

EdgeParams* ImageStack::getEdgeParams()
{
    return &m_edgeParams;
}

void ImageStack::preProcess()
{
    cv::Mat image = m_stack[Original].clone();
    if (image.channels() > 1)
        cv::cvtColor(image, image, CV_RGB2GRAY);
    if (m_commonParams.histogram)
        cv::equalizeHist(image, image);

    m_stack[PreProcess] = image;
    Q_EMIT(preProcessDone());
}

void ImageStack::detectEdges()
{
    cv::Mat image = m_stack[PreProcess].clone();

    if (m_commonParams.canny)
        cv::Canny(image, image, m_cannyParams.lowThreshold, m_cannyParams.lowThreshold*m_cannyParams.ratio, m_cannyParams.kernelSize);
    else
        cv::threshold(image, image, m_thresholdParams.thresh, m_thresholdParams.maxval, m_thresholdParams.type);

    m_stack[EdgeDetection] = image;
    Q_EMIT(detectEdgesDone());
}

void ImageStack::removePips()
{
    cv::Mat imageBin = m_stack[EdgeDetection].clone();

    if (m_commonParams.removepips) {
        QVector<Pip> pips = ImageStack::collectPips(imageBin);
        for (int i = 0; i < pips.size(); ++i) {
            cv::fillConvexPoly(imageBin, pips.at(i), cv::Scalar(0), 8, 0);
        }
        cv::polylines(imageBin, pips.toStdVector(), true, cv::Scalar(0), 3, CV_AA);
    }

    m_stack[RemovePips] = imageBin;
    Q_EMIT(removePipsDone());
}

void ImageStack::enhanceEdges()
{
    cv::Mat image = m_stack[RemovePips].clone();

    int size, type;

    size = m_edgeParams.dilateSize;
    type = m_edgeParams.dilateType;
    if (size) {
        cv::Mat element = cv::getStructuringElement(
                type,
                cv::Size(size, size),
                cv::Point(size/2, size/2)
        );
        cv::dilate(image, image, element);
    }

    size = m_edgeParams.dilateBlur;
    if (size)
        cv::blur(image, image, cv::Size(size, size), cv::Point(-1, -1));

    size = m_edgeParams.erodeSize;
    type = m_edgeParams.erodeType;
    if (size) {
        cv::Mat element = cv::getStructuringElement(
                type,
                cv::Size(size, size),
                cv::Point(size/2, size/2)
        );
        cv::erode(image, image, element);
    }

    size = m_edgeParams.erodeBlur;
    if (size)
        cv::blur(image, image, cv::Size(size, size), cv::Point(-1, -1));

    type = m_edgeParams.thresholdType;
    if (type >= 0)
        cv::threshold(image, image, 1, 255, type);

    m_stack[EdgeEnhancement] = image;
    Q_EMIT(enhanceEdgesDone());
}

void ImageStack::detectContours()
{
    cv::Mat image = m_stack[PreProcess].clone();
    cv::Mat imageBin = m_stack[EdgeEnhancement];

    cv::cvtColor(image, image, CV_GRAY2RGB);
    Outline* outline;
    QVector<Outline> outlines = collectOutlines(imageBin);
    for (int i = 0; i < outlines.size(); ++i) {
        outline = &outlines[i];
        cv::drawContours(image, std::vector<std::vector<cv::Point> >(1, outline->approx), -1, cv::Scalar(255, 0, 0), 1);
        cv::drawContours(image, std::vector<std::vector<cv::Point> >(1, outline->contour), -1, cv::Scalar(0, 0, 255), 2);
    }

    m_stack[ContourDetection] = image;
    Q_EMIT(detectContoursDone(outlines));
}


void ImageStack::detectFaces(QVector<Outline> outlines)
{
    cv::Mat image = m_stack[PreProcess].clone();

    bool storePips;

    QVector<Face> faces = ImageStack::collectFaces(outlines);
    if (faces.size() > 1) {
        storePips = false;
        for (int i = 0; i < faces.size(); ++i) {
            Face face = faces.at(i);
            image = face.draw(image);
        }
    } else {
        storePips = true;
        faces.clear();
        cv::Canny(image, image, 127, 255, 3);
        int size = 2;
        cv::Mat element = cv::getStructuringElement(
                cv::MORPH_RECT,
                cv::Size(size, size),
                cv::Point(size/2, size/2)
        );
        cv::dilate(image, image, element);
        cv::erode(image, image, element);

        QVector<Pip> pips = ImageStack::collectPips(image);
        for (int i = 0; i < pips.size(); ++i) {
            Pip pip = pips.at(i);
            cv::fillConvexPoly(image, pip, cv::Scalar(255), 8, 0);
            Face pipFace = Face(pip);
            faces.append(pipFace);
        }
        cv::polylines(image, pips.toStdVector(), true, cv::Scalar(0), 2, CV_AA);
        m_stack[EdgeDetection] = image;

        for (int i = 0; i < faces.size(); ++i)
            image = faces[i].draw(image);
    }

    m_stack[FaceDetection] = image;
    Q_EMIT(detectFacesDone(faces, storePips));
}

void ImageStack::detectCubes(QVector<Face> faces, bool storePips)
{
    cv::Mat image = m_stack[PreProcess].clone();

    QVector<Cube> cubes = QVector<Cube>::fromStdVector(Cube::collectCubes(faces.toStdVector(), storePips));
    for (int i = 0; i < cubes.size(); ++i) {
        Cube cube = cubes.at(i);
        cv::Scalar color = (i%2) ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 255, 255);
        image = cube.draw(image, color);
    }

    m_stack[CubeDetection] = image;
    Q_EMIT(detectCubesDone(cubes));
}

void ImageStack::detectTops(QVector<Cube> cubes)
{
    cv::Mat imageBin = m_stack[EdgeDetection].clone();
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
    Q_EMIT(detectTopsDone(topFaces));
}

void ImageStack::detectPips(QVector<cv::Mat> topFaces)
{
    cv::Mat image = m_stack[PreProcess].clone();

    QVector<Pip> pips;
    for (int i = 0; i < topFaces.size(); ++i) {
        cv::Mat topFace = topFaces.at(i);
        QVector<Pip> pipsOfTopFace = ImageStack::collectPips(topFace);
        if (pipsOfTopFace.size() == 0) {
            int size = 3;
            cv::Mat element = cv::getStructuringElement(
                    cv::MORPH_RECT,
                    cv::Size(size, size),
                    cv::Point(size/2, size/2)
            );
            cv::dilate(topFace, topFace, element);
            pipsOfTopFace = ImageStack::collectPips(topFace);
        }
        pips += pipsOfTopFace;
    }

    m_stack[PipDetection] = ImageStack::drawPips(image, pips);
    Q_EMIT(detectPipsDone(pips.size()));
}

void ImageStack::onCommonParamChanged(bool value)
{
    QObject* sender = QObject::sender();
    QString id = sender->objectName();

    if (id.compare("histogramCB") == 0)
        m_commonParams.histogram = value;
   else if (id.compare("pipCB") == 0)
        m_commonParams.removepips = value;

   preProcess();
}

void ImageStack::onThresholdParamChanged(int value)
{
    QObject* sender = QObject::sender();
    QString id = sender->objectName();

    m_commonParams.canny = false;

    if (id.compare("threshSlider") == 0)
        m_thresholdParams.thresh = value;
    else if (id.compare("maxvalSlider") == 0)
        m_thresholdParams.maxval = value;
    else if (id.compare("threshTypeGroup") == 0)
        m_thresholdParams.type = value;

    detectEdges();
}

void ImageStack::onCannyParamChanged(int value)
{
    QObject* sender = QObject::sender();
    QString id = sender->objectName();

    m_commonParams.canny = true;

    if (id.compare("lowThresholdSlider") == 0)
        m_cannyParams.lowThreshold = value;
    else if (id.compare("ratioSlider") == 0)
        m_cannyParams.ratio = value;
    else if (id.compare("kernelSizeSlider") == 0)
        m_cannyParams.kernelSize = value;

    detectEdges();
}

void ImageStack::onEdgeParamChanged(int value)
{
    QObject* sender = QObject::sender();
    QString id = sender->objectName();

    if (id.compare("dilateSizeSlider") == 0)
        m_edgeParams.dilateSize = value;
    else if(id.compare("dilateTypeGroup") == 0)
        m_edgeParams.dilateType = value;
    else if(id.compare("dilateBlurSlider") == 0)
        m_edgeParams.dilateBlur = value;
    else if(id.compare("erodeSizeSlider") == 0)
        m_edgeParams.erodeSize = value;
    else if(id.compare("erodeTypeGroup") == 0)
        m_edgeParams.erodeType = value;
    else if(id.compare("erodeBlurSlider") == 0)
        m_edgeParams.erodeBlur = value;
    else if(id.compare("ethreshTypeGroup") == 0)
        m_edgeParams.thresholdType = value;

    enhanceEdges();
}

using namespace cv;

QVector<Outline> ImageStack::collectOutlines(Mat imageBin)
{
    QVector<Outline> outlines;

    std::vector<Contour> contours;
    findContours(imageBin.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    Contour approx;
    for (size_t i = 0; i < contours.size(); ++i) {
        approxPolyDP(
                    Mat(contours[i]),
                    approx,
                    arcLength(Mat(contours[i]), true) * 0.02,
                    true
        );

        if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
            continue;

        // Do not store the border of the image
        if (approx.size() == 4 &&
                approx[0].x == 1 && approx[0].y == 1 &&
                approx[2].x >= imageBin.cols-5 && approx[2].y >= imageBin.rows-5) {
            continue;
         }

        outlines.append(Outline(contours[i], approx));
    }

    return outlines;
}

QVector<Face> ImageStack::collectFaces(QVector<Outline> outlines)
{
    QVector<Face> faces;

    Contour approx;
    for (int i = 0; i < outlines.size(); ++i) {
        approx = outlines[i].approx;
        if (approx.size() == 4)
            faces.append(Face(approx));
    }

    return faces;
}

QVector<Pip> ImageStack::collectPips(Mat image)
{
    QVector<Pip> pips;

    std::vector<Contour> contours;
    findContours(image.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

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

        if (approx.size() >= 7) {
            Moments m = moments(approx, true);
            Point center(m.m10/m.m00, m.m01/m.m00);
            bool inside = false;
            int j = 0;
            for (; j < pips.size(); ++j) {
                if (pointPolygonTest(pips[j], center, true) > 0) {
                    inside = true;
                    break;
                }
            }

            if (inside) {
                if (contourArea(approx) > contourArea(pips[j]))
                    pips.remove(j);
                else
                    continue;
            }

            pips.append(approx);
        }
    }

    return pips;
}

Mat ImageStack::drawPips(Mat image, QVector<Pip> pips, Scalar color)
{
    cvtColor(image, image, CV_GRAY2RGB);
    drawContours(image, pips.toStdVector(), -1, color, 2);
    return image;
}
