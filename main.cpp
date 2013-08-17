#include "ICLMainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ICLMainWindow w;
    w.show();
    
    return a.exec();
}
