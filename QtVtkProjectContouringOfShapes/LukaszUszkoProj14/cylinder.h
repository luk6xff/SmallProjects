#ifndef CYLINDER_H
#define CYLINDER_H

#include <shape.h>
#include <vtkCylinder.h>
#include <vtkRenderer.h>

class Cylinder:public Shape
{
public:

   Cylinder(vtkRenderer *handleRenderer);
   Cylinder(vtkRenderer *handleRenderer,QString name="CYLINDER");
   ~Cylinder();
   vtkImplicitFunction* getImplicitFunction(void);
   QGroupBox* getGivenShapeUI(void);

private:

  vtkSmartPointer<vtkCylinder> cylinder;
  QSlider*  cylinderRadiusSlider;
  QDoubleSpinBox* cylinderRadiusSpinBox;
  void loadDefaultSettings(void);


public slots:

  void setCylinderRadius(double radius);
   void refreshParametersOfGivenShape(void);

};

#endif // CYLINDER_H
