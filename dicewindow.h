#ifndef DICEWINDOW_H
#define DICEWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QVector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class ImageStack;
class ImageInfo;


namespace Ui {
class DiceWindow;
}

class DiceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DiceWindow(QWidget* parent = 0);
    ~DiceWindow();

public slots:
    void initImageStack(cv::Mat matImage);
    void showImageStack(int result);
    void onThreshChanged(int value);
    void onCannyChanged(int value);
    void onEdgeChanged(int value);

private:
    Ui::DiceWindow* m_ui;
    ImageStack* m_imageStack;
    ImageInfo* m_imageInfo;
    QVector<QLabel*> m_images;

    void initControllers();
    void updateInfo(int result);
};

#endif // DICESWINDOW_H
