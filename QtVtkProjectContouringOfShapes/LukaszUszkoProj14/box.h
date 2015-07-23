#ifndef BOX_H
#define BOX_H





#include <shape.h>
#include <vtkBox.h>
#include <vtkRenderer.h>

class Box:public Shape
{
public:

   Box(vtkRenderer *handleRenderer);
   Box(vtkRenderer *handleRenderer,QString name="Box");
   ~Box();
   vtkImplicitFunction* getImplicitFunction(void);
   QGroupBox* getGivenShapeUI(void);

private:

  vtkSmartPointer<vtkBox> box;
  QVector<QSpinBox*>* boxCoeffsSpinBoxTable;
  void loadDefaultSettings(void);
  void setConnections();


public slots:


   void refreshParametersOfGivenShape(void);

};


#endif // BOX_H
