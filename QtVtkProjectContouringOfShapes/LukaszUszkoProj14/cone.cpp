#include "cone.h"


Cone::Cone( vtkRenderer *handleRenderer):Shape(handleRenderer)
{

    cone=vtkSmartPointer<vtkCone>::New();

}


Cone::Cone(vtkRenderer *handleRenderer, QString name):Shape(name,handleRenderer)
{

    cone=vtkSmartPointer<vtkCone>::New();
    cone->SetAngle(20);

}

Cone::~Cone(){}

vtkImplicitFunction* Cone::getImplicitFunction(void){
    if(cone!=NULL)
    return cone;
    else return NULL;
}



 void Cone:: setConeRadius(double angle){

     if(cone!=NULL)
         cone->SetAngle(angle);
     emit(refreshChanges());
 }


 void Cone::refreshParametersOfGivenShape(void){
     if(cone!=NULL)
  //      cone->SetRadius(coneRadiusSlider->value());
     emit(refreshChanges());

 }

 QGroupBox* Cone:: getGivenShapeUI(void){
     givenShapeGroupBox = new QGroupBox(tr("Shape: Cone")); ;


       QVBoxLayout *coneVLayout= new QVBoxLayout;
       QHBoxLayout *coneHLayout= new QHBoxLayout;
       coneRadiusSlider=new QSlider(Qt::Horizontal);
       coneRadiusSpinBox=new QSpinBox();
       QLabel* coneRadiusTitle= new QLabel(tr("set Angle [0..100]:"));

       coneVLayout->addWidget(coneRadiusTitle);
       coneHLayout->addWidget(coneRadiusSlider);
      coneHLayout->addWidget(coneRadiusSpinBox);
       coneHLayout->insertSpacing(2,40);
       coneHLayout->insertSpacing(1,10);
       coneHLayout->insertSpacing(0,10);
       coneVLayout->addLayout(coneHLayout);
       givenShapeGroupBox->setLayout(coneVLayout);
       loadDefaultSettings();
       QObject::connect(coneRadiusSlider,SIGNAL(sliderMoved(int)),coneRadiusSpinBox,SLOT(setValue(int)));
       QObject::connect(coneRadiusSpinBox,SIGNAL(valueChanged(int)),coneRadiusSlider,SLOT(setValue(int)));
       QObject::connect(coneRadiusSpinBox,SIGNAL(valueChanged(int)),this,SLOT(refreshParametersOfGivenShape()));
       return givenShapeGroupBox;


 }


 void Cone::loadDefaultSettings(void){

     coneRadiusSlider->setValue(cone->GetAngle());

 }
