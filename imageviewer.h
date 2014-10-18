#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QObject>
#include <QModelIndex>

namespace Ui {
class DiceWindow;
}

namespace cv {
struct Mat;
}

class ImageExplorer;

class ImageViewer : public QObject
{
    Q_OBJECT

public:
    explicit ImageViewer(QObject *parent = 0);
    ImageViewer(Ui::DiceWindow* ui, ImageExplorer* explorer, QObject* parent = 0);

signals:

public slots:
    void load(QString path);
    void load(QModelIndex index);

private:
    Ui::DiceWindow* m_ui;
    ImageExplorer* m_explorer;

    QImage Mat2QImage(const cv::Mat& inMat);

};
#endif // IMAGEVIEWER_H
