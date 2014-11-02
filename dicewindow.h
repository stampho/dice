#ifndef DICEWINDOW_H
#define DICEWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QVector>

class ImageStack;
class DImage;

namespace Ui {
class DiceWindow;
}

class DiceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DiceWindow(QWidget *parent = 0);
    ~DiceWindow();

public slots:
    void initImageStack(DImage* dimage);
    void showImageStack(int phase, int result);

private:
    Ui::DiceWindow* m_ui;
    ImageStack* m_imageStack;
    QVector<QLabel*> m_images;
};

#endif // DICESWINDOW_H
