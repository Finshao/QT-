#include "kmainwindow.h"
//#include <vld.h>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    KMainWindow w;
    w.resize(1300, 800);
    w.show();
    return a.exec();
}
