#ifndef DICEWINDOW_H
#define DICEWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QVector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class ImageStack;

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
    void showImageStack(int phase, int result);
    void initControllers(int phase, int result);
    void onThreshChanged(int value);
    void onCannyChanged(int value);

private:
    Ui::DiceWindow* m_ui;
    ImageStack* m_imageStack;
    QVector<QLabel*> m_images;
};

#endif // DICESWINDOW_H
