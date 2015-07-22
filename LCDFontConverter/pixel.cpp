#include "pixel.h"
#include <QWidget>

Pixel::Pixel(QWidget *parent)
    : QWidget(parent)

{

    squareSize = 30;
    columns = 5;
    rows=8;
    clickx = -1;  // zmienna inf
    clicky= -1;
    setMouseTracking(true);
  ClearBothTab();// czyszcze tablice
 Wsk_Tab_Hex_Font = Tab_Hex_Font; // ustawiam wskaŸnik na zerowy elemnt tablicy
}
//***********************************************************************************************************************
QSize Pixel::sizeHint() const
{
   return QSize(columns*squareSize, rows*squareSize); // wielkoœæ kwadratu cza zainicjalizowaæ (funkcja sizeHint zwraca rozmiar danego widgetu , wiêcej w helpie)
}
//************************************************************************************************************************
void Pixel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) { // jeœli wcisne lewy klawisz myszy
        clickx=(event->x()/squareSize);  // zapisuje miejsce klikniêcia(wspó³rzêdne)
        clicky = (event->y()/squareSize);
        TabFont_5x8[clickx][clicky]^=1; // zmieniam dany piksel na przeciwn¹ wartoœæ
        for(int i=0;i<columns;i++){
        for(int j=0;j<rows;j++){
             if(TabFont_5x8[i][j]==true){Tab_Hex_Font[i]|=1<<j;}// tutaj zape³niam t¹ tablicê jedynk¹ w zale¿noœci od wartoœci tablicy o stanie pixela
            else Tab_Hex_Font[i]&= ~(1<<j); //skasuj bit
         }

      }// czyszcze tablice
       ///////////tutaj zamó¿dzyc cza i bedzie dobrze
            // ustawiam wskaŸnik na zerowym elemencie i rzutuje tablice charów na wskaŸnik na QByteArray

       emit setPixel(Wsk_Tab_Hex_Font);// emituje sygna³ który zmieni mi wartoœæ w line edicie
        update(); // aktualizuje od razu widget

    }
   else
       QWidget::mousePressEvent(event);
}
//*****************************************************************************************************************
void Pixel::paintEvent(QPaintEvent *event)
{
   QPainter painter(this);
  painter.fillRect(event->rect(), QBrush(Qt::white));


    QRect redrawRect = event->rect();
    int beginRow = redrawRect.top()/squareSize;/// wyznaczam se granica do rysowania kwadratów i ramek
    int endRow = redrawRect.bottom()/squareSize;
    int beginColumn = redrawRect.left()/squareSize;
    int endColumn = redrawRect.right()/squareSize;

    painter.setPen(QPen(Qt::gray));    // tutaj rysuje sobie kwadraty w widgecie
    for (int row = beginRow; row <= endRow; ++row) {
        for (int column = beginColumn; column <= endColumn; ++column) {
           painter.drawRect(column*squareSize, row*squareSize, squareSize, squareSize);
      }

    }
    for (int row = beginRow; row <= endRow; ++row) {

        for (int column = beginColumn; column <= endColumn; ++column) {

            if(TabFont_5x8[column][row]){// jeœli w tabeli jest prawda czyli zaznaczony pixel
            painter.setClipRect(column*squareSize, row*squareSize, squareSize, squareSize);
   painter.fillRect(column*squareSize + 1, row*squareSize + 1, squareSize, squareSize, QBrush(Qt::red));}
        }
    }
}
//****************************************************************************************
void Pixel::ClearBothTab(){ for(int i=0;i<columns;i++){
        Tab_Hex_Font[i]=0;
     for(int j=0;j<rows;j++){
         TabFont_5x8[i][j]=0;}}}// czyszcze tablice}
//QByteArray* Pixel::Bool_to_Hex_Font(bool(*wsk)[][8]){
//    for(int i)

//}









