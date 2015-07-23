#include "sphere.h"


Sphere::Sphere( vtkRenderer *handleRenderer):Shape(handleRenderer)
{

    sphere=vtkSmartPointer<vtkSphere>::New();
    sphere->SetRadius(0.5);
    sphere->SetCenter(0,0,0);
}


Sphere::Sphere(vtkRenderer *handleRenderer, QString name):Shape(name,handleRenderer)
{

    sphere=vtkSmartPointer<vtkSphere>::New();
    sphere->SetRadius(0.5);
    sphere->SetCenter(0,0,0);

}

Sphere::~Sphere(){}

vtkImplicitFunction* Sphere::getImplicitFunction(void){
    if(sphere!=NULL)
    return sphere;
    else return NULL;
}



 void Sphere:: setSphereRadius(double radius){

     if(sphere!=NULL)
         sphere->SetRadius(radius);
     emit(refreshChanges());
 }


 void Sphere::refreshParametersOfGivenShape(void){
     if(sphere!=NULL)
         sphere->SetRadius(sphereRadiusSpinBox->value());
     emit(refreshChanges());

 }

 QGroupBox* Sphere:: getGivenShapeUI(void){
     givenShapeGroupBox = new QGroupBox(tr("Shape: SPHERE")); ;

     //Sphere
       QVBoxLayout *sphereVLayout= new QVBoxLayout;
       QHBoxLayout *sphereHLayout= new QHBoxLayout;
    //   sphereRadiusSlider=new QSlider(Qt::Horizontal);
       sphereRadiusSpinBox=new QDoubleSpinBox();
       sphereRadiusSpinBox->setRange(0,1.0);
       sphereRadiusSpinBox->setSingleStep(0.01);
       QLabel* sphereRadiusTitle= new QLabel(tr("set Radius [0..1.0]:"));

       sphereHLayout->addWidget(sphereRadiusTitle);
      // sphereHLayout->addWidget(sphereRadiusSlider);
       sphereHLayout->addWidget(sphereRadiusSpinBox);
       sphereHLayout->insertSpacing(2,40);
       sphereHLayout->insertSpacing(1,10);
       sphereHLayout->insertSpacing(0,10);
       sphereVLayout->addLayout(sphereHLayout);
       givenShapeGroupBox->setLayout(sphereVLayout);
       loadDefaultSettings();
      // QObject::connect(sphereRadiusSlider,SIGNAL(sliderMoved(int)),sphereRadiusSpinBox,SLOT(setValue(int)));
      // QObject::connect(sphereRadiusSpinBox,SIGNAL(valueChanged(int)),sphereRadiusSlider,SLOT(setValue(int)));
       QObject::connect(sphereRadiusSpinBox,SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
       return givenShapeGroupBox;


 }


 void Sphere::loadDefaultSettings(void){

     sphereRadiusSpinBox->setValue(sphere->GetRadius());

 }
