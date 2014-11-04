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

    void init();

    enum Phase {
        Original,
        PreProcess,
        EdgeDetection,
        ContourDetection,
        FaceDetection,
        CubeDetection,
        TopDetection,
        PipDetection,
        PhaseCount
    };

    cv::Mat getImage(Phase phase);
    ThresholdParams* getThresholdParams();

signals:
    void preProcessDone(int phase);
    void detectEdgesDone(int phase);
    void detectContoursDone(int phase, QVector<Outline>);
    void detectFacesDone(int phase, QVector<Face>);
    void detectCubesDone(int phase, QVector<Cube>);
    void detectTopsDone(int phase, QVector<cv::Mat>);
    void detectPipsDone(int phase, int result);
    void ready(int phase, int result);

public slots:
    void preProcess();
    void detectEdges(int prev);
    void detectContours(int prev);
    void detectFaces(int prev, QVector<Outline>);
    void detectCubes(int prev, QVector<Face>);
    void detectTops(int prev, QVector<Cube>);
    void detectPips(int prev, QVector<cv::Mat>);

    void onThresholdParamChanged(int value);

private:
    cv::Mat m_stack[PhaseCount];
    ThresholdParams m_thresholdParams;

    static QVector<Outline> collectOutlines(cv::Mat imageBin);
    static QVector<Face> collectFaces(QVector<Outline> outlines);
    static QVector<Pip> collectPips(cv::Mat);
    static cv::Mat drawPips(cv::Mat image, QVector<Pip> pips, cv::Scalar color = cv::Scalar(0, 0, 255));
};

#endif // IMAGESTACK_H
