#ifndef SHAPEFACTORY_H
#define SHAPEFACTORY_H

#include <shape.h>
#include <sphere.h>
#include <quadric.h>
#include <cylinder.h>
#include <cone.h>
#include <box.h>

class ShapeFactory:public QObject
{
     Q_OBJECT
public:
    ShapeFactory(vtkRenderer* handleRenderer);
    ~ShapeFactory();

    Shape* getShape(QString shapeType);
    //ui
    QGroupBox* getChoiceOfShapesUI(void);

private slots:
    void sendSignalToGetShape();

private:
    //ui
    QGroupBox* chooseTheShapeGroupBox;
    vtkSmartPointer<vtkRenderer> handleRenderer;
    QComboBox* shapeBox;
    QPushButton* retrieveShapeButton;

signals:
    void retrieveProperShape(const QString& shapeType);

};

#endif // SHAPEFACTORY_H
