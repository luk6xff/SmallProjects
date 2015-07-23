#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowModified(false);

 //   w.move(300,300);
   // w.setWindowTitle("Test 14");
    w.show();
    
    return a.exec();
}
