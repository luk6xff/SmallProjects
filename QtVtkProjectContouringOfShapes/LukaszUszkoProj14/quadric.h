#ifndef QUADRIC_H
#define QUADRIC_H

#include <shape.h>
#include <vtkQuadric.h>
#include <vtkRenderer.h>



class Quadric: public Shape
{
public:
    Quadric();
    Quadric(vtkRenderer *handleRenderer);
    Quadric(vtkRenderer *handleRenderer,QString name="Quadric");
    ~Quadric();
    vtkImplicitFunction* getImplicitFunction(void);
    QGroupBox* getGivenShapeUI(void);

private:

  vtkSmartPointer<vtkQuadric> quadric;
  void setConnections(void);
  QVector<QDoubleSpinBox*>* quadricCoeffsSpinBoxTable;
  void loadDefaultSettings(void);


public slots:

  //void setQuadricCoefficients(QVector<double>& coeffs);
   void refreshParametersOfGivenShape(void);
};



#endif // QUADRIC_H
