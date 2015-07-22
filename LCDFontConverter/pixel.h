#include <QtGui>
#include <QSize>
#ifndef PIXEL_H
#define PIXEL_H
QT_BEGIN_NAMESPACE
class QMouseEvent;
class QPaintEvent;
QT_END_NAMESPACE

class Pixel:public QWidget
{
   Q_OBJECT
public:
    Pixel(QWidget *parent = 0); // konstruktor
    QSize sizeHint() const;  // funkcja wirtualna ustawiaj¹ca rozmiar wid¿etu , znaczy pikseli
    void ClearBothTab();

signals:
    void setPixel(quint8*);
protected:
    void mousePressEvent(QMouseEvent *event); // funkcja obs³uguj¹ca zdarzenie od klikniecia mysz¹ na poszczególny piksel
    void paintEvent(QPaintEvent *event); // funkcja obs³, zdarzenie od malowana
  //  QByteArray* Bool_to_Hex_Font(bool (*wsk)[][8]);  // funkcja zwraca mi wskaŸnik na tablicê ju¿ gotow¹ w formacie hex do wyswietlenia a jako argument wskaŸnik na tablice dwuwymiarow¹

private:
     int columns; // liczba kolumn
    int rows;    //liczba rzêdów
    int clickx,clicky; // zmienna informuj¹ca który klawisz wciœniêty ( na którym pikselu)
    int squareSize; // rozmiar poszczególnego kwadratu
    bool TabFont_5x8[5][8];//tablica przechowuj¹ca stan czy piksel zapalony czuy nie
   quint8 Tab_Hex_Font[5]; // tablica przechowuj¹ca mi hexa czcionki w line edicie
  quint8* Wsk_Tab_Hex_Font;

};

#endif // PIXEL_H

/*
#include <QFont>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QMouseEvent;
class QPaintEvent;
QT_END_NAMESPACE

//! [0]
class CharacterWidget : public QWidget
{
   Q_OBJECT

public:
    CharacterWidget(QWidget *parent = 0);
    QSize sizeHint() const;

public slots:
    void updateFont(const QFont &font);
    void updateSize(const QString &fontSize);
    void updateStyle(const QString &fontStyle);
    void updateFontMerging(bool enable);

signals:
    void characterSelected(const QString &character);

protected:
   // void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QFont displayFont;
    int columns;
    int lastKey;
    int squareSize;
};
//! [0]

#endif

  */
