#ifndef SHAPE_H
#define SHAPE_H

#include <vtkImplicitFunction.h>
#include <vtkSmartPointer.h>
#include <vtkSampleFunction.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <vtkContourFilter.h>
#include <vtkOutlineFilter.h>
#include <QString>
#include <QVector>
#include <QObject>
#include <QGroupBox>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
class Shape: public QObject
{

    Q_OBJECT
public:
     Shape(vtkRenderer *handleRenderer);
     Shape(QString name, vtkRenderer *handleRenderer);
     ~Shape();
     virtual vtkImplicitFunction* getImplicitFunction(void)=0;

     void sampleFuncSetup(vtkImplicitFunction* impFunc);

     //ui
     QGroupBox* getContourFilterUI(void);
   virtual QGroupBox* getGivenShapeUI(void)=0;
    virtual void loadDefaultSettings(void);
protected:
     QString nameOfShape;

     double numberOfContours;
     double rangeMin;
     double rangeMax;


     void addNewActor(QVector<vtkSmartPointer<vtkActor> > &actors, bool removeAllFirst);

     //ui
     QGroupBox* givenShapeGroupBox;

private:
     vtkSmartPointer<vtkSampleFunction > sampleShape;

     vtkSmartPointer<vtkContourFilter> contourShapeFilter;
     vtkSmartPointer<vtkPolyDataMapper> contourShapeMapper;
     vtkSmartPointer<vtkActor> contourShapeActor;

     vtkSmartPointer<vtkOutlineFilter> outLineShapeFilter;
     vtkSmartPointer<vtkPolyDataMapper> outLineShapeMapper;
     vtkSmartPointer<vtkActor> outLineShapeActor;

     vtkRenderer *handleRenderer;

     typedef QVector<vtkSmartPointer<vtkActor> > vecForActors;
 //ui
     QGroupBox* contourFiltersGroupBox;


     QSlider* contoursSlider;
     QSpinBox* contoursSpinBox;
     QDoubleSpinBox* startRangeSlider;
     QDoubleSpinBox* stopRangeSlider;

     QDoubleSpinBox* xSampleDimmensionSlider;
     QDoubleSpinBox* ySampleDimmensionSlider;
     QDoubleSpinBox* zSampleDimmensionSlider;


     QSlider* rColourSlider;
     QSlider* gColourSlider;
     QSlider* bColourSlider;

     void setConnections(void);

 public slots:
     void enableContourFilter(void);
     void setNumberOfCountours(int nrOfCounturs);
     void enableOutlineFilter();
     virtual void refreshParametersOfGivenShape(void)=0;
private slots:
     void setStartRange(double val);
     void setStopRange(double val);
     void setSampleFuncDimmension(void);
     void setColours(void);

 signals:
     void refreshChanges(void);  //to connect to main window's method to repaint qvtkWindow

};

#endif // SHAPE_H
