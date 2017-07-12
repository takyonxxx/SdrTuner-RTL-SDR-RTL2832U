#include "SdrWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SdrWindow w;
    w.show();
    int opt;
    while ((opt = getopt(argc, argv, "d:f:g:s:b:l:o:t:r:p:E:F:A:M:h")) != -1)
    {
       switch (opt) {
       case 'M':
           if (strcmp("wbfm",  optarg) == 0)
           {
           }
           break;
       case 'h':
       default:
           break;
       }
    }
    return a.exec();
}
