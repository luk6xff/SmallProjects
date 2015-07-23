#include "quadric.h"
#include "QVector"
Quadric::Quadric( vtkRenderer *handleRenderer):Shape(handleRenderer)
{

    quadric=vtkSmartPointer<vtkQuadric>::New();
    quadric->SetCoefficients(10,10,10,0,0,30,30,30,0,0);

}


Quadric::Quadric(vtkRenderer *handleRenderer, QString name):Shape(name,handleRenderer)
{

     quadric=vtkSmartPointer<vtkQuadric>::New();
     quadric->SetCoefficients(1,0.8,1,0,0,0.4,0,0.6,0,0);

}

Quadric::~Quadric(){}

vtkImplicitFunction* Quadric::getImplicitFunction(void){
    if(quadric!=NULL)
    return quadric;
    else return NULL;
}


/*
 void Quadric:: setQuadricCoefficients(QVector<double>& coeffs){

     if(quadric!=NULL&&coeffs.size()==10)
         quadric->SetCoefficients(coeffs.data()
                     );
         emit(refreshChanges());
 }

*/

 void Quadric::refreshParametersOfGivenShape(void){

     if(quadric!=NULL)
         quadric->SetCoefficients(quadricCoeffsSpinBoxTable->at(0)->value(),
                                  quadricCoeffsSpinBoxTable->at(1)->value(),
                                  quadricCoeffsSpinBoxTable->at(2)->value(),
                                  quadricCoeffsSpinBoxTable->at(3)->value(),
                                  quadricCoeffsSpinBoxTable->at(4)->value(),
                                  quadricCoeffsSpinBoxTable->at(5)->value(),
                                  quadricCoeffsSpinBoxTable->at(6)->value(),
                                  quadricCoeffsSpinBoxTable->at(7)->value(),
                                  quadricCoeffsSpinBoxTable->at(8)->value(),
                                  quadricCoeffsSpinBoxTable->at(9)->value()
                    );
         emit(refreshChanges());
 }


 QGroupBox* Quadric:: getGivenShapeUI(void){
     givenShapeGroupBox = new QGroupBox(tr("Shape: QUADRIC")); ;


       const char* quadricCoeffsPattern[]= {"*x^2","*y^2", "*z^2" ,"*x*y","*y*z","*x*z","*x","*y","*z","*1"};
       QVBoxLayout *quadricVLayout= new QVBoxLayout;
       QHBoxLayout *quadricHLayout1= new QHBoxLayout;
       QHBoxLayout *quadricHLayout2= new QHBoxLayout;
       quadricCoeffsSpinBoxTable =new QVector<QDoubleSpinBox*>();
       QLabel* quadricRadiusTitle= new QLabel(tr("set Coefficients F(x,y,z)="));

       quadricVLayout->addWidget(quadricRadiusTitle);
       for(int i=0;i<5;i++){
        quadricCoeffsSpinBoxTable->push_back(new QDoubleSpinBox());
        quadricCoeffsSpinBoxTable->at(i)->setRange(-50,50);
         quadricCoeffsSpinBoxTable->at(i)->setSingleStep(0.5);
       quadricHLayout1->addWidget(quadricCoeffsSpinBoxTable->at(i));
       quadricHLayout1->addWidget(new QLabel(QString(quadricCoeffsPattern[i])));
       }

       for(int i=5;i<10;i++){
           quadricCoeffsSpinBoxTable->push_back(new QDoubleSpinBox());
           quadricCoeffsSpinBoxTable->at(i)->setRange(-50,50);
            quadricCoeffsSpinBoxTable->at(i)->setSingleStep(0.5);
       quadricHLayout2->addWidget(quadricCoeffsSpinBoxTable->at(i));
       quadricHLayout2->addWidget(new QLabel(QString(quadricCoeffsPattern[i])));
       }

     //  quadricHLayout->insertSpacing(2,40);
     //  quadricHLayout->insertSpacing(1,10);
      // quadricHLayout->insertSpacing(0,10);
       quadricVLayout->addLayout(quadricHLayout1);
       quadricVLayout->addLayout(quadricHLayout2);
       givenShapeGroupBox->setLayout(quadricVLayout);
       loadDefaultSettings();
       setConnections();

       return givenShapeGroupBox;


 }


  void Quadric::setConnections(void){

      QObject::connect(quadricCoeffsSpinBoxTable->at(0),SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
      QObject::connect(quadricCoeffsSpinBoxTable->at(1),SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
      QObject::connect(quadricCoeffsSpinBoxTable->at(2),SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
      QObject::connect(quadricCoeffsSpinBoxTable->at(3),SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
      QObject::connect(quadricCoeffsSpinBoxTable->at(4),SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
      QObject::connect(quadricCoeffsSpinBoxTable->at(5),SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
      QObject::connect(quadricCoeffsSpinBoxTable->at(6),SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
      QObject::connect(quadricCoeffsSpinBoxTable->at(7),SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
      QObject::connect(quadricCoeffsSpinBoxTable->at(8),SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));
      QObject::connect(quadricCoeffsSpinBoxTable->at(9),SIGNAL(valueChanged(double)),this,SLOT(refreshParametersOfGivenShape()));

  }


  void Quadric::loadDefaultSettings(void){

      double* data = quadric->GetCoefficients();
      quadricCoeffsSpinBoxTable->at(0)->setValue(data[0]);
      quadricCoeffsSpinBoxTable->at(1)->setValue(data[1]);
      quadricCoeffsSpinBoxTable->at(2)->setValue(data[2]);
      quadricCoeffsSpinBoxTable->at(3)->setValue(data[3]);
      quadricCoeffsSpinBoxTable->at(4)->setValue(data[4]);
      quadricCoeffsSpinBoxTable->at(5)->setValue(data[5]);
      quadricCoeffsSpinBoxTable->at(6)->setValue(data[6]);
      quadricCoeffsSpinBoxTable->at(7)->setValue(data[7]);
      quadricCoeffsSpinBoxTable->at(8)->setValue(data[8]);
      quadricCoeffsSpinBoxTable->at(9)->setValue(data[9]);


  }
