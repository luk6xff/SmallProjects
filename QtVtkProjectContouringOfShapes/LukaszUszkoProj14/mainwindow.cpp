#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vtkSampleFunction.h>
#include <vtkPolyDataReader.h>
#include <vtkImageViewer.h>
#include <vtkPNGWriter.h>
#include <vtkPNGReader.h>
#include <vtkPDBReader.h>
#include <vtkDICOMImageReader.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkImageMapper.h>

#include <vtkLODActor.h>
#include <vtkGlyph3D.h>
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>

#include <QFileDialog>


#define DEBUG_ENABLED

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(trUtf8("Projekt 14 -Lukasz Uszko"));

    this->setFixedSize(this->size().width(),this->size().height());

    // utworzenie renderera - wirtualnej sceny
    ren = vtkRenderer::New();

    ui->qvtkWidget->GetRenderWindow()->AddRenderer(ren);    // dodanie sceny do widgetu!
    renWin = ui->qvtkWidget->GetRenderWindow();             // pobranie wskaźnika do widgetu
    shape=NULL;
    hLayout=NULL;

    factory = new ShapeFactory(ren);
    QObject::connect(factory,SIGNAL(retrieveProperShape(const QString&)),this,SLOT(newShape(const QString&)));
    ui->dockWidgetRightSide->setWidget(factory->getChoiceOfShapesUI());

setMenuOptions();
////////////////////////////////////////////////////////////////
// Zestawienie połączeń SIGNAL->SLOT interfejsu Qt
   setConnections();

}


MainWindow::~MainWindow(){
    delete shape;
    delete factory;
    ren->Delete();
    delete ui;
}

void MainWindow::showOnStatusBar(int i)
{

    ui->statusBar->showMessage(QString::fromUtf8("Wartość: %1").arg(i));
}


void MainWindow::updateCoords()
{
    double camPosition[3];
    ren->GetActiveCamera()->GetPosition(camPosition);   // pobranie wsp. kamery

    QString str = QString("x=%1 : y=%2 : z=%3")
                  .arg(camPosition[0]).arg( camPosition[1]).arg(camPosition[2]);

    ui->statusBar->showMessage(str);
}


// slot do obslugi "postepow w obliczeniach" wysylanych z obiektow VTK
void MainWindow::updateProgressValue( vtkObject *caller,
                                      unsigned long vtkEvent,
                                      void* client_data,
                                      void* call_data)
{

        if( vtkEvent == vtkCommand::ProgressEvent )     // weryfikacja zdarzenia
        {
                double v = *((double*) call_data);
                int value = 100*v;
                emit newProgress( value );  // wysłanie sygnału - powiadomienie o postępach w %
        }
        QCoreApplication::processEvents();  // "odblokowanie" kolejki zdarzeń w GUI
}

void MainWindow::repaintMainQVTKWidget(void){

      ui->qvtkWidget->repaint();

}




void MainWindow::newShape(const QString &newShape)
{


    if(shape!=NULL){
    delete shape;
        delete hLayout;
      //  delete tempWidget;
    ren->EraseOn();
    ren->RemoveAllViewProps();}
    shape = factory->getShape(newShape);
    shape->sampleFuncSetup(shape->getImplicitFunction());
    connect(shape,SIGNAL(refreshChanges()),this,SLOT(repaintMainQVTKWidget()));
    //repaintMainQVTKWidget();
    ui->qvtkWidget->clearFocus();
    ui->qvtkWidget->adjustSize();

    hLayout = new QHBoxLayout;

    tempWidget = new QWidget();
    scrollLLayout = new QVBoxLayout;
    scrollRLayout = new QVBoxLayout;
        scrollLLayout->addWidget(shape->getContourFilterUI());
        scrollLLayout->addWidget(shape->getGivenShapeUI());

        hLayout->addLayout(scrollLLayout);
        hLayout->addLayout(scrollRLayout);
        ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        ui->scrollArea->setMaximumSize(600,150);
        ui->scrollArea->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum);
        ui->scrollArea->setWidgetResizable(true);

        tempWidget->setLayout(hLayout);
  ui->scrollArea->setWidget(tempWidget);
       // ui->scrollArea->widget()->setLayout(hLayout);
        ui->scrollArea->resize(1,10);
        ui->qvtkWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        ui->centralWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

}


void MainWindow::setConnections()
{
    connect(this, SIGNAL(setStatusText(QString)),
            ui->statusBar, SLOT(showMessage(QString)) );

    connect(ui->qvtkWidget, SIGNAL(mouseEvent(QMouseEvent*)),
            this, SLOT(updateCoords()));

    vtkSmartPointer<vtkEventQtSlotConnect> connections =
            vtkSmartPointer<vtkEventQtSlotConnect>::New();

    connections->Connect(ui->qvtkWidget->GetInteractor(),
                            vtkCommand::LeftButtonPressEvent,
                            this,
                            SLOT(updateCoords()));



    //przykład podpięcia się pod postępy w obliczeniach, np. pod wczytywanie pliku:
    //    connections->Connect( XXXReader,
    //                          vtkCommand::ProgressEvent,
    //                          this,
    //                          SLOT( updateProgressValue( vtkObject*, unsigned long, void*, void*)));

    // debug poprawności połączeń na konsolę:
    connections->PrintSelf(cout, vtkIndent());  // tak można podglądać dowolne obiekty VTK!
}


void MainWindow::setMenuOptions(){

    QMenu *fileMenu= new QMenu(tr("&File"),this);
    QAction *openFileAction= fileMenu->addAction(tr("&Open..."));
    openFileAction->setShortcuts(QKeySequence::Open);

    QAction *saveFileAction= fileMenu->addAction(tr("&Save As..."));
    saveFileAction->setShortcuts(QKeySequence::SaveAs);

    fileMenu->addSeparator();
    QAction *quitFileAction= fileMenu->addAction(tr("&Exit"));
    quitFileAction->setShortcuts(QKeySequence::Quit);

    QMenu *helpMenu= new QMenu(tr("&Help"),this);
    QAction *aboutAction= helpMenu->addAction(tr("&About"));



    connect(openFileAction,SIGNAL(triggered()),this,SLOT(openFile()));
    connect(saveFileAction,SIGNAL(triggered()),this,SLOT(saveFile()));
    connect(quitFileAction,SIGNAL(triggered()),qApp,SLOT(quit()));



    connect(aboutAction,SIGNAL(triggered()),this,SLOT(aboutMessage()));

    ui->menuBar->addMenu(fileMenu);
    ui->menuBar->addMenu(helpMenu);



    QMenuBar* bar = new QMenuBar();
    ui->mainToolBar->addWidget(bar);
    bar->addMenu(fileMenu);
    bar->addMenu(helpMenu);

}


void MainWindow::aboutMessage(){


    QMessageBox::about(this,tr("O programie..."),QString::fromUtf8(

                           "<p>Demo klasy vtkContourFilter</p>"
                           "<p>Wizualizacja w systemach biomedycznych</p>"
                           "<p align=\"right\"> autor: Lukasz Uszko 2014 </p>"
                           ));
}


bool MainWindow::saveFile(){

    vtkSmartPointer<vtkWindowToImageFilter> win2imgFilter= vtkSmartPointer<vtkWindowToImageFilter>::New();
    win2imgFilter->SetInput(renWin);
    win2imgFilter->SetMagnification(3); // the resolution of the output image 3 times bigger
    win2imgFilter->SetInputBufferTypeToRGBA(); //
    win2imgFilter->Update();
    vtkSmartPointer<vtkPNGWriter> writer= vtkSmartPointer<vtkPNGWriter>::New();
    QString imageName = QFileDialog::getSaveFileName(this,tr("Save image"),"picture",tr("vtkImages (*.png)"));
    writer->SetInputConnection(win2imgFilter->GetOutputPort());
    writer->SetFileName(imageName.toAscii().data());
    writer->Write();
    return true;

}



void  MainWindow::openFile(){

    QString imageName = QFileDialog::getOpenFileName(this,tr("Openimage"),"/home",tr("vtkImages (*.png *.vtk *.pdb *.dcm *.img)"));
   if(!imageName.isEmpty())
       loadImage(imageName);

}


void MainWindow::loadImage(const QString& imageName){

    QFile file(imageName);

    if(!file.open(QFile::ReadOnly)){
        QMessageBox::warning(this,tr("App"),tr("Cannot read file %1: \n%2.").arg(imageName)
                                                                                 .arg(file.errorString()));

        return ;

    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    loadProperTypeOfFile(imageName);


     QApplication::restoreOverrideCursor();
     statusBar()->showMessage(tr("File  has been loaded!"), 2000);



}




QString MainWindow::checkTypeOfReadFile(const QString& nameOfFile){

    QString result;

    for(QString::const_iterator it=nameOfFile.end()-3;it!=nameOfFile.end();++it){

        result+=*it;

    }
#ifdef DEBUG_ENABLED
qDebug()<<result;
#endif

    return result;

}



 bool MainWindow::loadProperTypeOfFile(const QString& imageName){
     QString typeOfFile;
     typeOfFile= checkTypeOfReadFile(imageName);
if(typeOfFile== "vtk"){

    vtkSmartPointer<vtkGenericDataObjectReader> dataReader =vtkSmartPointer<vtkGenericDataObjectReader>::New();
    dataReader->SetFileName(imageName.toAscii().data());
    dataReader->Update();

    contourFilter =vtkSmartPointer<vtkContourFilter>::New();
    contourFilter->SetInputConnection(dataReader->GetOutputPort());
    contourFilter->GenerateValues(2,0,5.0);

    contourMapper=vtkSmartPointer<vtkPolyDataMapper>::New();
    contourMapper->SetInputConnection(contourFilter->GetOutputPort());
    contourMapper->SetScalarRange(0,1.2);

    contourActor=vtkSmartPointer<vtkActor>::New();
    contourActor->SetMapper(contourMapper);
    contourActor->GetProperty()->SetColor(3,0,0);

    outLineFilter = vtkSmartPointer<vtkOutlineFilter> ::New();
    outLineFilter->SetInputConnection(dataReader->GetOutputPort());
    outLineMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
    outLineMapper->SetInputConnection(outLineFilter->GetOutputPort());

    outLineActor=  vtkSmartPointer<vtkActor>::New();
    outLineActor->SetMapper(outLineMapper);
    outLineActor->GetProperty()->SetColor(0,0,0);

    ren->RemoveAllViewProps();
    ren->ResetCamera();
    ren->AddActor(contourActor);
     showTheWindowThatProjectHasNotBeenFinishedYet();
}

else if(typeOfFile== "pdb"){
    vtkSmartPointer<vtkPDBReader> dataReader = vtkSmartPointer<vtkPDBReader>::New();
    dataReader->SetFileName(imageName.toAscii().data());
    dataReader->SetBScale(1.0);
    dataReader->SetHBScale(1.0);
    dataReader->Update();

    double resolution= 20;//std::sqrt(m) TODO

    vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();

    sphere->SetCenter(0,0,0);
    sphere->SetRadius(1);
    sphere->SetThetaResolution(static_cast<int>(resolution));
    sphere->SetPhiResolution(static_cast<int>(resolution));

    vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputConnection(dataReader->GetOutputPort());
    glyph->SetColorMode(1);
    glyph->ScalingOn();
    glyph->SetScaleMode(2);
    glyph->SetScaleFactor(.25);
    glyph->SetSourceConnection(sphere->GetOutputPort());



    vtkSmartPointer<vtkPolyDataMapper> atomMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    atomMapper->SetInputConnection(glyph->GetOutputPort());
    atomMapper->ImmediateModeRenderingOn();
    atomMapper->UseLookupTableScalarRangeOff();
    atomMapper->ScalarVisibilityOn();
    atomMapper->SetScalarModeToDefault();


    vtkSmartPointer<vtkLODActor> atom= vtkSmartPointer<vtkLODActor>::New();
    atom->SetMapper(atomMapper);
    atom->GetProperty()->SetRepresentationToSurface();
    atom->GetProperty()->SetInterpolationToGouraud();
    atom->GetProperty()->SetAmbient(0.15);
    atom->GetProperty()->SetDiffuse(0.85);
    atom->GetProperty()->SetSpecular(0.1);
    atom->GetProperty()->SetSpecularPower(30);
    atom->GetProperty()->SetSpecularColor(1,1,1);
    atom->SetNumberOfCloudPoints(30000);

    ren->RemoveAllViewProps();
    ren->ResetCamera();
    ren->AddActor(atom);

    vtkSmartPointer<vtkTubeFilter> tube= vtkSmartPointer<vtkTubeFilter>::New();
    tube->SetInputConnection(dataReader->GetOutputPort());
    tube->SetNumberOfSides(static_cast<int>(resolution));
    tube->CappingOff();
    tube->SetRadius(0.2);
    tube->SetVaryRadius(0);
    tube->SetRadiusFactor(10);

    vtkSmartPointer<vtkPolyDataMapper> bondMapper= vtkSmartPointer<vtkPolyDataMapper>::New();
    bondMapper->SetInputConnection(tube->GetOutputPort());
    bondMapper->ImmediateModeRenderingOn();
    bondMapper->UseLookupTableScalarRangeOff();
    bondMapper->ScalarVisibilityOff();
    bondMapper->SetScalarModeToDefault();

    vtkSmartPointer<vtkLODActor> bond= vtkSmartPointer<vtkLODActor>::New();
    bond->SetMapper(bondMapper);
    bond->GetProperty()->SetRepresentationToSurface();
    bond->GetProperty()->SetInterpolationToGouraud();
    bond->GetProperty()->SetAmbient(0.15);
    bond->GetProperty()->SetDiffuse(0.85);
    bond->GetProperty()->SetSpecular(0.1);
    bond->GetProperty()->SetSpecularPower(30);
    bond->GetProperty()->SetSpecularColor(1,1,1);

   ren->AddActor(bond);

   contourFilter =vtkSmartPointer<vtkContourFilter>::New();
   contourFilter->SetInputConnection(glyph->GetOutputPort());
   contourFilter->GenerateValues(2,0,5.0);

   contourMapper=vtkSmartPointer<vtkPolyDataMapper>::New();
   contourMapper->SetInputConnection(contourFilter->GetOutputPort());
   contourMapper->SetScalarRange(0,1.2);

   contourActor=vtkSmartPointer<vtkActor>::New();
   contourActor->SetMapper(contourMapper);
   contourActor->GetProperty()->SetColor(3,0,0);

   outLineFilter = vtkSmartPointer<vtkOutlineFilter> ::New();
   outLineFilter->SetInputConnection(glyph->GetOutputPort());
   outLineMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
   outLineMapper->SetInputConnection(outLineFilter->GetOutputPort());

   outLineActor=  vtkSmartPointer<vtkActor>::New();
   outLineActor->SetMapper(outLineMapper);
   outLineActor->GetProperty()->SetColor(0,0,0);

   ren->AddActor(contourActor);
 showTheWindowThatProjectHasNotBeenFinishedYet();


}
else if(typeOfFile== "png"){
    vtkSmartPointer<vtkPNGReader> dataReader=vtkSmartPointer<vtkPNGReader>::New();
    dataReader->SetFileName(imageName.toAscii().data());
    dataReader->Update();

    contourFilter =vtkSmartPointer<vtkContourFilter>::New();
    contourFilter->SetInputConnection(dataReader->GetOutputPort());
    contourFilter->GenerateValues(2,0,5.0);

    contourMapper=vtkSmartPointer<vtkPolyDataMapper>::New();
    contourMapper->SetInputConnection(contourFilter->GetOutputPort());
    contourMapper->SetScalarRange(0,1.2);

    contourActor=vtkSmartPointer<vtkActor>::New();
    contourActor->SetMapper(contourMapper);
    contourActor->GetProperty()->SetColor(3,0,0);

    outLineFilter = vtkSmartPointer<vtkOutlineFilter> ::New();
    outLineFilter->SetInputConnection(dataReader->GetOutputPort());
    outLineMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
    outLineMapper->SetInputConnection(outLineFilter->GetOutputPort());

    outLineActor=  vtkSmartPointer<vtkActor>::New();
    outLineActor->SetMapper(outLineMapper);
    outLineActor->GetProperty()->SetColor(0,0,0);

    ren->RemoveAllViewProps();
    ren->ResetCamera();
    ren->AddActor(contourActor);
 showTheWindowThatProjectHasNotBeenFinishedYet();
}
else if(typeOfFile== "dcm"){   showTheWindowThatProjectHasNotBeenFinishedYet();   }
else if(typeOfFile==  "img"){  showTheWindowThatProjectHasNotBeenFinishedYet();  }

else return false;


   ren->SetBackground2(0.6, 0.6, 0.6);
   ren->SetBackground(.1, .2, .3);
   ren->GradientBackgroundOn();
   ren->ResetCamera();
   ren->GetRenderWindow()->Render();


return true;


 }


 void MainWindow::showTheWindowThatProjectHasNotBeenFinishedYet(void)
 {
     QMessageBox msgBox;
     msgBox.setWindowTitle("ALERT!");
     msgBox.setIcon(QMessageBox::Warning);
     msgBox.setText(tr("This feature is not supprted yet"));
     msgBox.setDefaultButton(QMessageBox::Ok);
     msgBox.exec();

 }


void MainWindow::on_actionZamknij_triggered()
{
    qApp->exit();
}
