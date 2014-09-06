#include "diceswindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DicesWindow w;
    w.show();

    return a.exec();
}
