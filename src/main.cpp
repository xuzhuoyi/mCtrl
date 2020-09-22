#include "motioncontrol.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MotionControl w;
    w.show();
    return a.exec();
}
