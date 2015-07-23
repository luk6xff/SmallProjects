#include "box.h"


Box::Box( vtkRenderer *handleRenderer):Shape(handleRenderer)
{

    box=vtkSmartPointer<vtkBox>::New();

}


Box::Box(vtkRenderer *handleRenderer, QString name):Shape(name,handleRenderer)
{

    box=vtkSmartPointer<vtkBox>::New();
    box->SetBounds(0,5,0,5,0,5);

}

Box::~Box(){}

vtkImplicitFunction* Box::getImplicitFunction(void){
    if(box!=NULL)
    return box;
    else return NULL;
}


 void Box::refreshParametersOfGivenShape(void){
     if(box!=NULL)
       {
        box->SetBounds(boxCoeffsSpinBoxTable->at(0)->value(),
                       boxCoeffsSpinBoxTable->at(1)->value(),
                       boxCoeffsSpinBoxTable->at(2)->value(),
                       boxCoeffsSpinBoxTable->at(3)->value(),
                       boxCoeffsSpinBoxTable->at(4)->value(),
                       boxCoeffsSpinBoxTable->at(5)->value()
                       );
                     }
     emit(refreshChanges());
 }

 QGroupBox* Box:: getGivenShapeUI(void){
     givenShapeGroupBox = new QGroupBox(tr("Shape: BOX")); ;


     const char* boxCoeffsPattern[]= {"xMin","xMax", "yMin" ,"yMax","zMin","zMax"};
     QVBoxLayout *boxVLayout= new QVBoxLayout;
     QHBoxLayout *boxHLayout1= new QHBoxLayout;
     QHBoxLayout *boxHLayout2= new QHBoxLayout;
     boxCoeffsSpinBoxTable =new QVector<QSpinBox*>();
     QLabel* boxTitle= new QLabel(tr("set Bounds of the box"));

     boxVLayout->addWidget(boxTitle);
     for(int i=0;i<3;i++){
      boxCoeffsSpinBoxTable->push_back(new QSpinBox());
      boxCoeffsSpinBoxTable->at(i)->setRange(-50,50);
     boxHLayout1->addWidget(boxCoeffsSpinBoxTable->at(i));
     boxHLayout1->addWidget(new QLabel(QString(boxCoeffsPattern[i])));
     }

     for(int i=3;i<6;i++){
         boxCoeffsSpinBoxTable->push_back(new QSpinBox());
         boxCoeffsSpinBoxTable->at(i)->setRange(-50,50);
     boxHLayout2->addWidget(boxCoeffsSpinBoxTable->at(i));
    boxHLayout2->addWidget(new QLabel(QString(boxCoeffsPattern[i])));
     }

   //  quadricHLayout->insertSpacing(2,40);
   //  quadricHLayout->insertSpacing(1,10);
    // quadricHLayout->insertSpacing(0,10);
     boxVLayout->addLayout(boxHLayout1);
     boxVLayout->addLayout(boxHLayout2);
     givenShapeGroupBox->setLayout(boxVLayout);
     loadDefaultSettings();
     setConnections();

       return givenShapeGroupBox;


 }

 void Box::setConnections(void){

     QObject::connect(boxCoeffsSpinBoxTable->at(0),SIGNAL(valueChanged(int)),this,SLOT(refreshParametersOfGivenShape()));
     QObject::connect(boxCoeffsSpinBoxTable->at(1),SIGNAL(valueChanged(int)),this,SLOT(refreshParametersOfGivenShape()));
     QObject::connect(boxCoeffsSpinBoxTable->at(2),SIGNAL(valueChanged(int)),this,SLOT(refreshParametersOfGivenShape()));
     QObject::connect(boxCoeffsSpinBoxTable->at(3),SIGNAL(valueChanged(int)),this,SLOT(refreshParametersOfGivenShape()));
     QObject::connect(boxCoeffsSpinBoxTable->at(4),SIGNAL(valueChanged(int)),this,SLOT(refreshParametersOfGivenShape()));
     QObject::connect(boxCoeffsSpinBoxTable->at(5),SIGNAL(valueChanged(int)),this,SLOT(refreshParametersOfGivenShape()));


 }


 void Box::loadDefaultSettings(void){
    double* data = box->GetBounds(
                 );
    boxCoeffsSpinBoxTable->at(0)->setValue(data[0]);
    boxCoeffsSpinBoxTable->at(1)->setValue(data[1]);
    boxCoeffsSpinBoxTable->at(2)->setValue(data[2]);
    boxCoeffsSpinBoxTable->at(3)->setValue(data[3]);
    boxCoeffsSpinBoxTable->at(4)->setValue(data[4]);
    boxCoeffsSpinBoxTable->at(5)->setValue(data[5]);
}
