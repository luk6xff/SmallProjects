#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
QT_BEGIN_NAMESPACE  // nie muszê includowaæ jak tak zrobiê
class QLineEdit;
class QScrollArea;
class QPushButton;
QT_END_NAMESPACE
class Pixel;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void insertChar(quint8*);
   void ClearTabFont(Pixel& klasa);
private:
    Ui::MainWindow *ui;

   Pixel* newpixel;
     QString hex,przecinek,zero;
    QLineEdit *lineEdit;
    QScrollArea *scrollArea;
    QPushButton *ClearButton; // przycisk do czyszczenia ca³ej tablicy

};

#endif // MAINWINDOW_H


/**************************************************************************************************888
#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
class QClipboard;
class QComboBox;
class QFontComboBox;
class QLineEdit;
class QScrollArea;
class QCheckBox;
QT_END_NAMESPACE
class CharacterWidget;

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

public slots:
    void findStyles(const QFont &font);
    void findSizes(const QFont &font);
 void insertCharacter(const QString &character);
    void updateClipboard();

private:
    CharacterWidget *characterWidget;
    QClipboard *clipboard;
    QComboBox *styleCombo;
    QComboBox *sizeCombo;
    QFontComboBox *fontCombo;
    QLineEdit *lineEdit;
    QScrollArea *scrollArea;
    QCheckBox *fontMerging;
};
//! [0]
*/
