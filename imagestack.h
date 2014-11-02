#ifndef IMAGESTACK_H
#define IMAGESTACK_H

#include <QObject>
#include <QVector>

#include "API/dimage.h"
#include "API/face.h"
#include "API/cube.h"

class ImageStack : public QObject
{
    Q_OBJECT
public:
    explicit ImageStack(QObject* parent = 0);
    ImageStack(DImage* image, QObject* parent = 0);

    enum Phase {
        Original,
        PreProcess,
        EdgeDetection,
        FaceDetection,
        CubeDetection,
        TopDetection,
        PipDetection,
        PhaseCount
    };

    DImage* getImage(Phase phase);

signals:
    void preProcessDone(int phase);
    void detectEdgesDone(int phase);
    void detectFacesDone(int phase, QVector<Face>);
    void detectCubesDone(int phase, QVector<Cube>);
    void detectTopsDone(int phase, QVector<DImage>);
    void detectPipsDone(int phase, int result);
    void ready(int phase, int result);

public slots:
    void preProcess();
    void detectEdges(int prev);
    void detectFaces(int prev);
    void detectCubes(int prev, QVector<Face>);
    void detectTops(int prev, QVector<Cube>);
    void detectPips(int prev, QVector<DImage>);

private:
    DImage m_stack[PhaseCount];

};

#endif // IMAGESTACK_H
