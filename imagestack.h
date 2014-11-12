#ifndef IMAGESTACK_H
#define IMAGESTACK_H

#include <QObject>
#include <QVector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "API/core.h"
#include "API/cube.h"
#include "API/face.h"

struct ThresholdParams {
    int thresh;
    int maxval;
    int type;
};

struct CannyParams {
    int lowThreshold;
    int ratio;
    int kernelSize;
};

struct Outline {
    Outline()
    {}

    Outline(Contour contour, Contour approx)
        : contour(contour)
        , approx(approx)
    {}

    Contour contour;
    Contour approx;
};

class ImageStack : public QObject
{
    Q_OBJECT
public:
    explicit ImageStack(QObject* parent = 0);
    ImageStack(cv::Mat image, QObject* parent = 0);
    ~ImageStack();

    enum Phase {
        Original,
        PreProcess,
        EdgeDetection,
        RemovePips,
        EdgeEnhancement,
        ContourDetection,
        FaceDetection,
        CubeDetection,
        TopDetection,
        PipDetection,
        PhaseCount
    };

    cv::Mat getImage(Phase phase);
    ThresholdParams* getThresholdParams();
    CannyParams* getCannyParams();

signals:
    void preProcessDone();
    void detectEdgesDone();
    void enhanceEdgesDone();
    void removePipsDone();
    void detectContoursDone(QVector<Outline>);
    void detectFacesDone(QVector<Face>, bool);
    void detectCubesDone(QVector<Cube>);
    void detectTopsDone(QVector<cv::Mat>);
    void detectPipsDone(int result);

    void ready(int result);

public slots:
    void preProcess();
    void detectEdges();
    void enhanceEdges();
    void removePips();
    void detectContours();
    void detectFaces(QVector<Outline>);
    void detectCubes(QVector<Face>, bool);
    void detectTops(QVector<Cube>);
    void detectPips(QVector<cv::Mat>);

    void onThresholdParamChanged(int value);
    void onCannyParamChanged(int value);

private:
    cv::Mat m_stack[PhaseCount];

    ThresholdParams m_thresholdParams;
    CannyParams m_cannyParams;

    bool m_cannyEnabled;

    static QVector<Outline> collectOutlines(cv::Mat imageBin);
    static QVector<Face> collectFaces(QVector<Outline> outlines);
    static QVector<Pip> collectPips(cv::Mat);
    static cv::Mat drawPips(cv::Mat image, QVector<Pip> pips, cv::Scalar color = cv::Scalar(0, 0, 255));
};

#endif // IMAGESTACK_H
