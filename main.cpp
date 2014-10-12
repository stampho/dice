#include "dicewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DiceWindow w;
    w.show();

    return a.exec();
}
