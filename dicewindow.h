#ifndef DICEWINDOW_H
#define DICEWINDOW_H

#include <QMainWindow>

namespace Ui {
class DiceWindow;
}

class DiceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DiceWindow(QWidget *parent = 0);
    ~DiceWindow();

private:
    Ui::DiceWindow *ui;
};

#endif // DICESWINDOW_H
