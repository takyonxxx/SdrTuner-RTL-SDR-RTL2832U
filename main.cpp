#include "SdrWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SdrWindow w;
    w.show();    
    return a.exec();
}
