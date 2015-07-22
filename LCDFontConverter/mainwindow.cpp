#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pixel.h"
#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{  // ui->setupUi(this);
    hex="0x"; przecinek=',';zero="0";
    QWidget *centralWidget = new QWidget;
    QVBoxLayout *lajut=new QVBoxLayout;
    lineEdit=new QLineEdit;
    scrollArea = new QScrollArea;
    newpixel= new Pixel;

 //  scrollArea->resize(60,60);
    scrollArea->setWidget(newpixel);
    lajut->addWidget(scrollArea);
    lineEdit->adjustSize();
    lajut->addWidget(lineEdit);
    centralWidget->setLayout(lajut);
    setCentralWidget(centralWidget);
    setWindowTitle(tr("czcionko-dizajner"));
    connect(newpixel,SIGNAL(setPixel(quint8*)),this,SLOT(insertChar(quint8*))); // ³¹cze ustawienie piksela ze zmian¹ lineEditu
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::insertChar(quint8 *tab)
{  // lineEdit->setText("const czcionka[5]={");
   lineEdit->clear();
   lineEdit->setText("const char tab[]={}");
   lineEdit->setCursorPosition(18);

   quint8 data;
   QByteArray Tab_Bytes; // tworze tablice bajtow po to ¿eby móc na postaæ hex zamieniæ
     for(int i=0;i<5;i++){
         Tab_Bytes[i]=*tab++;   // zapisuje QByte Arraya liczbami quint8
        }
   QByteArray Tab_Hex;  // tutaj tworze QbyteArray w postaci Hex zwracana referencja
     for(int i=0;i<5;i++,*tab++){
         data=Tab_Bytes.at(i); // zwracam wartosci poszcegolnych bitow
         Tab_Hex=Tab_Bytes.number(data,16).toUpper(); // zamieniam na 16 widok
         if(i<4){if(Tab_Bytes[i]&(1<<7)){lineEdit->insert(hex+(QString)(Tab_Hex)+przecinek);} // tu maskuje 0 bo mi wyrzuca³ bo QByteArray jest 16 bit
                 else if(Tab_Bytes[i]>=(char)0x10){
                 lineEdit->insert(hex+(QString)(Tab_Hex)+przecinek);}
                 else lineEdit->insert(hex+zero+(QString)(Tab_Hex)+przecinek);
         }
         else {if(Tab_Bytes[i]&(1<<7)){lineEdit->insert(hex+(QString)(Tab_Hex));}
               else if(Tab_Bytes[i]>=(char)0x10){
                 lineEdit->insert(hex+(QString)(Tab_Hex));}
               else lineEdit->insert(hex+zero+(QString)(Tab_Hex));
         }
        }

}
 void MainWindow::ClearTabFont(Pixel &klasa){
    klasa.ClearBothTab();
 }

