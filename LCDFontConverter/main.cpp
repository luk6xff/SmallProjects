#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;  // tworze main window
    w.show(); // wyswietlam
    
    return a.exec();  // co by siemi nie zamyka³o
}
