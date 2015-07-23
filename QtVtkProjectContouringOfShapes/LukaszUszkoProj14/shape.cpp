#include "shape.h"
#include <vtkProperty.h>
#include <QDebug>




Shape::Shape(vtkRenderer *handleRenderer)
{
  this->handleRenderer=handleRenderer;
  sampleShape=vtkSmartPointer<vtkSampleFunction >::New();
  nameOfShape="ANY SHAPE";
  numberOfContours=2;
  rangeMin=0;
  rangeMax=2.0;
}


Shape::Shape(QString name, vtkRenderer *handleRenderer):nameOfShape(name)
{
 // handleRenderer= vtkRenderer::New();
  this->handleRenderer= handleRenderer;
  sampleShape=vtkSmartPointer<vtkSampleFunction >::New();
  numberOfContours=2;
  rangeMin=0;
  rangeMax=2.0;

}

Shape::~Shape(){}

void Shape::sampleFuncSetup(vtkImplicitFunction* impFunc){

  sampleShape->SetSampleDimensions(50,50,50);
    sampleShape->SetImplicitFunction(impFunc);

    ////////FOR TEST  MAYBE ONLY
    if(sampleShape!=NULL){
     contourShapeFilter=  vtkSmartPointer<vtkContourFilter>::New() ;
     contourShapeMapper=  vtkSmartPointer<vtkPolyDataMapper>::New() ;
     contourShapeActor=   vtkSmartPointer<vtkActor>::New() ;


    contourShapeFilter->SetInputConnection(sampleShape->GetOutputPort());

    contourShapeFilter->GenerateValues(3,rangeMin,rangeMax);

    contourShapeMapper->SetInputConnection(contourShapeFilter->GetOutputPort());
    contourShapeMapper->SetScalarRange(0,1.5);
    contourShapeMapper->SetColorModeToMapScalars();

    contourShapeActor->SetMapper(contourShapeMapper);
    contourShapeActor->GetProperty()->SetColor(20,49,70);
    vecForActors vec;
    vec.push_back(contourShapeActor);
    addNewActor(vec,false);
    //enableOutlineFilter();
    }
}
//SLOTS

void Shape::enableContourFilter(){

    if(sampleShape!=NULL){
     contourShapeFilter=  vtkSmartPointer<vtkContourFilter>::New() ;
     contourShapeMapper=  vtkSmartPointer<vtkPolyDataMapper>::New() ;
     contourShapeActor=   vtkSmartPointer<vtkActor>::New() ;
     contourShapeFilter->SetInputConnection(sampleShape->GetOutputPort());
     contourShapeFilter->GenerateValues(numberOfContours,rangeMin,rangeMax);

     contourShapeMapper->SetInputConnection(contourShapeFilter->GetOutputPort());
     contourShapeMapper->SetScalarRange(0,1.5);

     contourShapeActor->SetMapper(contourShapeMapper);
     contourShapeActor->GetProperty()->SetColor(4,1,1);
     vecForActors vec;
     vec.push_back(contourShapeActor);
     addNewActor(vec,false);
    }
}




void Shape::enableOutlineFilter(){


    if(sampleShape!=NULL){
    outLineShapeFilter= vtkSmartPointer<vtkOutlineFilter>::New() ;
    outLineShapeMapper= vtkSmartPointer<vtkPolyDataMapper>::New() ;
    outLineShapeActor= vtkSmartPointer<vtkActor>::New() ;

    outLineShapeFilter = vtkSmartPointer<vtkOutlineFilter> ::New();
    outLineShapeFilter->SetInputConnection(sampleShape->GetOutputPort());
    outLineShapeMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
    outLineShapeMapper->SetInput(outLineShapeFilter->GetOutput());
    //actors
    outLineShapeActor=  vtkSmartPointer<vtkActor>::New();
    outLineShapeActor->SetMapper(outLineShapeMapper);
    outLineShapeActor->GetProperty()->SetColor(3,5,80);
    vecForActors vec;
    vec.push_back(outLineShapeActor);
    addNewActor(vec,false);
    emit(refreshChanges());

    }
}


void Shape::setNumberOfCountours(int nrOfCounturs){
   if(contourShapeFilter!=NULL){
   numberOfContours= nrOfCounturs;
   contourShapeFilter->GenerateValues(nrOfCounturs,rangeMin,rangeMax);
   emit(refreshChanges());
   }
}



void Shape::setStartRange(double val){
    if(contourShapeFilter!=NULL){
        rangeMin=val;
        contourShapeFilter->GenerateValues(numberOfContours,rangeMin,rangeMax);
        emit(refreshChanges());
    }
}
void Shape::setStopRange(double val){
    if(contourShapeFilter!=NULL){
        rangeMax=val;
        contourShapeFilter->GenerateValues(numberOfContours,rangeMin,rangeMax);
        emit(refreshChanges());
    }
}

void Shape:: setSampleFuncDimmension(void){
   if(sampleShape!=NULL){
    sampleShape->SetSampleDimensions(xSampleDimmensionSlider->value(),
                                      ySampleDimmensionSlider->value(),
                                      zSampleDimmensionSlider->value());
    emit(refreshChanges());
   }
}




void Shape::setColours(){
     contourShapeActor->GetProperty()->SetColor(rColourSlider->value(),
                                                gColourSlider->value(),
                                                bColourSlider->value());
     emit(refreshChanges());
}

// END OF SLOTS
void Shape::addNewActor(QVector<vtkSmartPointer<vtkActor> > &actors, bool removeAllFirst){


    if(handleRenderer!=NULL){

        if(removeAllFirst){
        handleRenderer->RemoveAllViewProps();
        handleRenderer->ResetCamera();

         }
        for(int i=0;i<actors.size();i++){

            handleRenderer->AddActor(actors.at(i));
        }

        handleRenderer->SetBackground2(0.8, 0.7, 0.6);
        handleRenderer->SetBackground(.5, .7, .9);
        handleRenderer->GradientBackgroundOn();
        handleRenderer->ResetCamera();
        handleRenderer->GetRenderWindow()->Render();
    }


}

QGroupBox* Shape:: getContourFilterUI(void){
contourFiltersGroupBox = new QGroupBox(tr("ContourFilter_Menu"));
QVBoxLayout *cFVLayout= new QVBoxLayout;
QHBoxLayout *cFHLayout1= new QHBoxLayout;
QHBoxLayout *cFHLayout2= new QHBoxLayout;
QHBoxLayout *cFHLayout3= new QHBoxLayout;
QHBoxLayout *cFHLayout4= new QHBoxLayout;
QHBoxLayout *cFHLayout5= new QHBoxLayout;

contoursSlider=new QSlider(Qt::Horizontal);
contoursSpinBox=new QSpinBox();
startRangeSlider=new QDoubleSpinBox();
startRangeSlider->setRange(0,5);
startRangeSlider->setSingleStep(0.1);
stopRangeSlider=new QDoubleSpinBox();
stopRangeSlider->setRange(0,5);
stopRangeSlider->setSingleStep(0.1);

xSampleDimmensionSlider=new QDoubleSpinBox();
ySampleDimmensionSlider=new QDoubleSpinBox();
zSampleDimmensionSlider=new QDoubleSpinBox();

rColourSlider=new QSlider(Qt::Horizontal);
gColourSlider=new QSlider(Qt::Horizontal);
bColourSlider=new QSlider(Qt::Horizontal);

QLabel* cFTitleNumberOfCountours= new QLabel(tr("nr of countours [0..10]:"));
QLabel* cFTitleMinValue= new QLabel(tr("min Value:"));
QLabel* cFTitleMaxValue= new QLabel(tr("max Value"));

QLabel* cFTitleSampleDimmension= new QLabel(tr("dimmensions:"));
QLabel* cFxSampleDimmension= new QLabel(tr("x:"));
QLabel* cFySampleDimmension= new QLabel(tr("y:"));
QLabel* cFzSampleDimmension= new QLabel(tr("z:"));

QLabel* cFTitleColours= new QLabel(tr("colours:"));
QLabel* cFRColours= new QLabel(tr("R:"));
QLabel* cFGColours= new QLabel(tr("G:"));
QLabel* cFBColours= new QLabel(tr("B:"));

// for (int i = 0; i < 10; ++i) {
//     coeffsSlider[i] = new QSlider(Qt::Horizontal);
 cFHLayout1->addWidget(cFTitleNumberOfCountours)   ;
 contoursSlider->setRange(0,10); //cuz of my laptop was being dead with bigger values
 contoursSpinBox->setRange(0,10);
 cFHLayout1->addWidget(contoursSlider);
 cFHLayout1->addWidget(contoursSpinBox);
 cFVLayout->addLayout(cFHLayout1);
 cFHLayout2->addWidget(cFTitleMinValue)   ;
 cFHLayout2->addWidget(startRangeSlider);
 cFHLayout2->addWidget(cFTitleMaxValue);
 cFHLayout2->addWidget(stopRangeSlider);
 cFVLayout->addLayout(cFHLayout2);

 cFHLayout4->addWidget(cFTitleSampleDimmension);
 cFHLayout4->addSpacing(50);
 cFHLayout4->addWidget(cFxSampleDimmension);
 cFHLayout4->addWidget(xSampleDimmensionSlider);
 cFHLayout4->addWidget(cFySampleDimmension);
 cFHLayout4->addWidget(ySampleDimmensionSlider);
 cFHLayout4->addWidget(cFzSampleDimmension);
 cFHLayout4->addWidget(zSampleDimmensionSlider);
 cFHLayout4->addSpacing(50);
 cFVLayout->addLayout(cFHLayout4);


 /*
 cFHLayout5->addWidget(cFTitleColours);
 cFHLayout5->addWidget(cFRColours);
 cFHLayout5->addWidget(rColourSlider);
 cFHLayout5->addWidget(cFGColours);
 cFHLayout5->addWidget(gColourSlider);
 cFHLayout5->addWidget(cFBColours);
 cFHLayout5->addWidget(bColourSlider);
 cFVLayout->addLayout(cFHLayout5);
 */

 contourFiltersGroupBox->setLayout( cFVLayout);
 Shape::loadDefaultSettings();
 setConnections();
 return contourFiltersGroupBox;
}


void Shape:: setConnections(void){
 QObject::connect(contoursSlider,SIGNAL(sliderMoved(int)),contoursSpinBox,SLOT(setValue(int)));
 QObject::connect(contoursSpinBox,SIGNAL(valueChanged(int)),contoursSlider,SLOT(setValue(int)));
 QObject::connect(contoursSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setNumberOfCountours(int)));
 QObject::connect(startRangeSlider,SIGNAL(valueChanged(double)),this,SLOT(setStartRange(double)));
 QObject::connect(stopRangeSlider,SIGNAL(valueChanged(double)),this,SLOT(setStopRange(double)));
 QObject::connect(xSampleDimmensionSlider,SIGNAL(valueChanged(double)),this,SLOT(setSampleFuncDimmension()));
 QObject::connect(ySampleDimmensionSlider,SIGNAL(valueChanged(double)),this,SLOT(setSampleFuncDimmension()));
 QObject::connect(zSampleDimmensionSlider,SIGNAL(valueChanged(double)),this,SLOT(setSampleFuncDimmension()));
 QObject::connect(rColourSlider,SIGNAL(valueChanged(int)),this,SLOT(setColours()));
 QObject::connect(gColourSlider,SIGNAL(valueChanged(int)),this,SLOT(setColours()));
 QObject::connect(bColourSlider,SIGNAL(valueChanged(int)),this,SLOT(setColours()));



}



void Shape::loadDefaultSettings(void){

    contoursSpinBox->setValue(this->numberOfContours);
    contoursSlider->setValue(this->numberOfContours);
    startRangeSlider->setValue(this->rangeMin);
    stopRangeSlider->setValue(this->rangeMax);
    xSampleDimmensionSlider->setValue(sampleShape->GetSampleDimensions()[0]);
    ySampleDimmensionSlider->setValue(sampleShape->GetSampleDimensions()[1]);
    zSampleDimmensionSlider->setValue(sampleShape->GetSampleDimensions()[2]);
   // rColourSlider->setValue(TODO);
   // gColourSlider->setValue(TODO);
   // bColourSlider->setValue(TODO);

}





