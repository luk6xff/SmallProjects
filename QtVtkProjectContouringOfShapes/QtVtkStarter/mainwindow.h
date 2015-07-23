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

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleUnicam.h>

#include <vtkEventQtSlotConnect.h>  // połączenie VTK->Qt

#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkObject.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void newMesh();             // inicjalizacja strumienia VTK
    void setConnections();      // inicjalizacja połoczeń SIGNAL-SLOT

signals:
    void setStatusText(QString);// wystanie tekstu na StatusBar
    void newProgress(int);      // powiedomienie o postępach w obliczeniach

public slots:
    void showOnStatusBar(int);  // wysłanie wartości INT na StatusBar
    void updateCoords();        // VTK -> Qt
    void updateProgressValue( vtkObject *, unsigned long,
                              void* client_data, void* call_data); // VTK -> Qt
    void setResolution(int);    // Qt -> VTK


private slots:
    void on_actionZamknij_triggered();

private:
    Ui::MainWindow *ui;         // wyklikany interfejs użytkownika

    vtkConeSource* source;      // ewskaźniki na obiekty VTK
    vtkPolyDataMapper* mapper;
    vtkActor* actor;
    vtkRenderer* ren;
    vtkRenderWindow* renWin;
    QVTKInteractor* iren;

};

#endif // MAINWINDOW_H
