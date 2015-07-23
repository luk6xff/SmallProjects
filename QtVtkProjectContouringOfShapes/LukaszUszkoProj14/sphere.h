#ifndef SPHERE_H
#define SPHERE_H
#include <shape.h>
#include <vtkSphere.h>
#include <vtkRenderer.h>

class Sphere:public Shape
{
public:

   Sphere(vtkRenderer *handleRenderer);
   Sphere(vtkRenderer *handleRenderer,QString name="SPHERE");
   ~Sphere();
   vtkImplicitFunction* getImplicitFunction(void);
   QGroupBox* getGivenShapeUI(void);

private:

  vtkSmartPointer<vtkSphere> sphere;
  QSlider*  sphereRadiusSlider;
  QDoubleSpinBox* sphereRadiusSpinBox;
  void loadDefaultSettings(void);


public slots:

  void setSphereRadius(double radius);
  void refreshParametersOfGivenShape(void);

};

#endif // SPHERE_H
