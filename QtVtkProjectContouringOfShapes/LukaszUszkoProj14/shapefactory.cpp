#include "shapefactory.h"

ShapeFactory::ShapeFactory(vtkRenderer *handleRenderer):handleRenderer(handleRenderer){


}

ShapeFactory::~ShapeFactory(){//delete handleRenderer;
                             }

Shape* ShapeFactory::getShape(QString shapeType){

    if(shapeType=="SPHERE") return (new Sphere(handleRenderer,shapeType));
    else if(shapeType=="QUADRIC") return (new Quadric(handleRenderer,shapeType));
    else if(shapeType=="CONE") return (new Cone(handleRenderer,shapeType));
    else if(shapeType=="CYLINDER") return (new Cylinder(handleRenderer,shapeType));
    else if(shapeType=="BOX") return (new Box(handleRenderer,shapeType));
    else return NULL;
}


////////////////choose a proper shape group Box

QGroupBox* ShapeFactory:: getChoiceOfShapesUI(void){
chooseTheShapeGroupBox = new QGroupBox("Choose the object");
shapeBox =new QComboBox();
shapeBox = new QComboBox;
shapeBox->addItem(("SPHERE"));
shapeBox->addItem(("QUADRIC"));
shapeBox->addItem(("CYLINDER"));
shapeBox->addItem(("CONE"));
shapeBox->addItem(("BOX"));

retrieveShapeButton = new QPushButton(("Retrieve")) ;
QVBoxLayout *cSVLayout= new QVBoxLayout;

cSVLayout->addWidget(shapeBox,1,Qt::AlignHCenter);
cSVLayout->addWidget(retrieveShapeButton,1,Qt::AlignHCenter);
chooseTheShapeGroupBox->setLayout(cSVLayout);
QObject::connect(retrieveShapeButton,SIGNAL(clicked()),this,SLOT(sendSignalToGetShape()));
return chooseTheShapeGroupBox;

}

void ShapeFactory::sendSignalToGetShape(){

   QString shapeType= shapeBox->currentText();
    emit(retrieveProperShape(shapeType));

}
