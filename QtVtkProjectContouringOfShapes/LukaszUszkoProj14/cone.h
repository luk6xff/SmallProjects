#ifndef CONE_H
#define CONE_H


#include <shape.h>
#include <vtkCone.h>
#include <vtkRenderer.h>

class Cone:public Shape
{
public:

   Cone(vtkRenderer *handleRenderer);
   Cone(vtkRenderer *handleRenderer,QString name="Cone");
   ~Cone();
   vtkImplicitFunction* getImplicitFunction(void);
   QGroupBox* getGivenShapeUI(void);

private:

  vtkSmartPointer<vtkCone> cone;
  QSlider*  coneRadiusSlider;
  QSpinBox* coneRadiusSpinBox;
  void loadDefaultSettings(void);


public slots:

  void setConeRadius(double radius);
  void refreshParametersOfGivenShape(void);

};

#endif // CONE_H
