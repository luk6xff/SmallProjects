#include "cylinder.h"



Cylinder::Cylinder( vtkRenderer *handleRenderer):Shape(handleRenderer)
{

   cylinder=vtkSmartPointer<vtkCylinder>::New();

}


Cylinder::Cylinder(vtkRenderer *handleRenderer, QString name):Shape(name,handleRenderer)
{

  cylinder=vtkSmartPointer<vtkCylinder>::New();
  cylinder->SetRadius(0.5);

}

Cylinder::~Cylinder(){}

vtkImplicitFunction* Cylinder::getImplicitFunction(void){
    if(cylinder!=NULL)
    return cylinder;
    else return NULL;
}



 void Cylinder:: setCylinderRadius(double radius){

     if(cylinder!=NULL)
         cylinder->SetRadius(radius);
     emit(refreshChanges());
 }


 void Cylinder::refreshParametersOfGivenShape(void){
     if(cylinder!=NULL)
        cylinder->SetRadius(cylinderRadiusSlider->value());
     emit(refreshChanges());

 }

 QGroupBox* Cylinder:: getGivenShapeUI(void){
     givenShapeGroupBox = new QGroupBox(tr("Shape: CYLINDER"));

     //cylinder
       QVBoxLayout *cylinderVLayout= new QVBoxLayout;
       QHBoxLayout *cylinderHLayout= new QHBoxLayout;
       cylinderRadiusSlider=new QSlider(Qt::Horizontal);
       cylinderRadiusSpinBox=new QDoubleSpinBox();
       cylinderRadiusSpinBox->setRange(0,1.0);
       cylinderRadiusSpinBox->setSingleStep(0.01);
       QLabel* cylinderRadiusTitle= new QLabel(tr("set Radius [0..1.0]:"));

      cylinderHLayout->addWidget(cylinderRadiusTitle);
       //cylinderHLayout->addWidget(cylinderRadiusSlider);
       cylinderHLayout->addWidget(cylinderRadiusSpinBox);
       cylinderHLayout->insertSpacing(2,40);
       cylinderHLayout->insertSpacing(1,10);
       cylinderHLayout->insertSpacing(0,10);
       cylinderVLayout->addLayout(cylinderHLayout);
       givenShapeGroupBox->setLayout(cylinderVLayout);
       loadDefaultSettings();
     //  QObject::connect(cylinderRadiusSlider,SIGNAL(sliderMoved(int)),cylinderRadiusSpinBox,SLOT(setValue(double))); //TODO write a class QDoubleSlider
    //   QObject::connect(cylinderRadiusSpinBox,SIGNAL(valueChanged(double),cylinderRadiusSlider,SLOT(setValue(int)));
       QObject::connect(cylinderRadiusSpinBox,SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
       return givenShapeGroupBox;


 }


 void Cylinder::loadDefaultSettings(void){

     cylinderRadiusSpinBox->setValue(cylinder->GetRadius());

 }
