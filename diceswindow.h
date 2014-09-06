#ifndef DICESWINDOW_H
#define DICESWINDOW_H

#include <QMainWindow>

namespace Ui {
class DicesWindow;
}

class DicesWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DicesWindow(QWidget *parent = 0);
    ~DicesWindow();

private:
    Ui::DicesWindow *ui;
};

#endif // DICESWINDOW_H
