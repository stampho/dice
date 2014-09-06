#include "diceswindow.h"
#include "ui_diceswindow.h"

DicesWindow::DicesWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DicesWindow)
{
    ui->setupUi(this);
}

DicesWindow::~DicesWindow()
{
    delete ui;
}
