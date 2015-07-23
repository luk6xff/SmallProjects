/*
Projekt Startowy prezentujący możliwości połączenia Qt i VTK

Mirosław Socha
Katedra Metrologii i Elektroniki AGH
Kraków 2014

*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QVTKWidget.h>
#include <QMessageBox>

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleUnicam.h>
#include <vtkContourFilter.h>
#include <vtkOutlineFilter.h> 

#include <vtkEventQtSlotConnect.h>  // połączenie VTK->Qt

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkObject.h>

#include <vtkJPEGReader.h>
#include <vtkJPEGWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkRenderLargeImage.h>

#include <shapefactory.h>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


protected:       
    void setConnections();      // inicjalizacja połoczeń SIGNAL-SLOT
    void setMenuOptions();
    void loadImage(const QString& imageName);

signals:
    void setStatusText(QString);// wystanie tekstu na StatusBar
    void newProgress(int);      // powiedomienie o postępach w obliczeniach

public slots:
    void newShape(const QString& newShape);
    void showOnStatusBar(int);  // wysłanie wartości INT na StatusBar
    void updateCoords();
    void updateProgressValue( vtkObject *, unsigned long,
                              void* client_data, void* call_data); // VTK -> Qt
    void repaintMainQVTKWidget(void);

private slots:
    void on_actionZamknij_triggered();
    void aboutMessage(); //prints a message box ABOUT
    bool saveFile();
    void openFile();

private:
    Ui::MainWindow *ui;

    vtkPolyDataMapper* mapper;
    vtkActor* actor;
    vtkRenderer* ren;
    vtkRenderWindow* renWin;
    QVTKInteractor* iren;



    vtkSmartPointer<vtkContourFilter> contourFilter;
    vtkSmartPointer<vtkPolyDataMapper> contourMapper;
    vtkSmartPointer<vtkActor> contourActor;

    vtkSmartPointer<vtkOutlineFilter> outLineFilter;
    vtkSmartPointer<vtkPolyDataMapper> outLineMapper;
     vtkSmartPointer<vtkActor> outLineActor;


    ShapeFactory* factory;
    Shape* shape;
    QHBoxLayout*hLayout ;
    QWidget* tempWidget;
    QVBoxLayout*scrollLLayout;
    QVBoxLayout*scrollRLayout;

    QString checkTypeOfReadFile(const QString& nameOfFile);

    bool loadProperTypeOfFile(const QString& imageName);

    static const QVector<QString>* handledFiles;
    void showTheWindowThatProjectHasNotBeenFinishedYet(void);

};

#endif // MAINWINDOW_H
