#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(trUtf8("QtVtkStarter"));

////////////////////////////////////////////////////////////////
// Obsługa VTK

    // utworzenie renderera - wirtualnej sceny
    ren = vtkRenderer::New();

    // podłączenie renderera do wigetu:
    ui->qvtkWidget->GetRenderWindow()->AddRenderer(ren);    // dodanie sceny do widgetu!
    renWin = ui->qvtkWidget->GetRenderWindow();             // pobranie wskaźnika do widgetu

    // utowrzenie strumienia wizualziacji:
    //  * inicjalizacja obiektów VTK
    //  * podłączenie przetwarzania danych do wizualziacji (do okna wizualizacji)
    newMesh(); // przykładowy strumień VTK

////////////////////////////////////////////////////////////////
// Zestawienie połączeń SIGNAL->SLOT interfejsu Qt
    setConnections();

}

void MainWindow::showOnStatusBar(int i)
{
    // wystawienie wartości INT na StatusBar
    ui->statusBar->showMessage(QString::fromUtf8("Wartość: %1").arg(i));
}

// pobranie wsp. kamery
void MainWindow::updateCoords()
{
    double camPosition[3];
    ren->GetActiveCamera()->GetPosition(camPosition);   // pobranie wsp. kamery

    QString str = QString("x=%1 : y=%2 : z=%3")
                  .arg(camPosition[0]).arg( camPosition[1]).arg(camPosition[2]);
//    qDebug() << str;      // wyslanie str na konsole
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

// zmiana parametrów obiektu VTK
void MainWindow::setResolution(int res)
{
    if (source!=NULL)                   // sprawdzenie czy obiekt istnieje!!!
    {
        source->SetResolution(res);     // zmiana parametru obiektu VTK
        ui->qvtkWidget->repaint();      // wymuszenie odświerzenia okna VTK
    }
}

// metoda inicjująca strumień obiektów VTK,
// wywoływana JEDNOKROTNIE w konstruktorze klasy
// jest to przeniesienie funkcjonalności poza konstruktor
void MainWindow::newMesh()
{
    // Geometria - proceduralnie generowany ostroslup:
    source = vtkConeSource::New();
      source->SetRadius(3);
      source->SetHeight(5);
      source->SetResolution(5);

    //// od mappera zaczyna się część wizualizująca
    // Mapper
    mapper = vtkPolyDataMapper::New();
      mapper->ImmediateModeRenderingOn();
      mapper->SetInputConnection(source->GetOutputPort());

    // Aktor - graficzna reprezentacja "obiektu" na wirtualnej scenie
    actor = vtkActor::New();
      actor->SetMapper(mapper);

    // dodanie Aktora do renderer'a (wirtualnej sceny)
    ren->AddActor(actor);

    ren->SetBackground2(0.6, 0.6, 0.6);
    ren->SetBackground(0.1, 0.1, 0.15);
    ren->GradientBackgroundOn();

    // Reset kamery
    ren->ResetCamera();
    ren->GetRenderWindow()->Render();

    // Sprzatanie - zezwolenie na usunięcie danych, gdy będą niepotrzebne
    actor->Delete();
    mapper->Delete();
    source->Delete();
}

// zestawianie połączeń sygnałów i slotów
// wywoływana JEDNOKROTNIE w konstruktorze klasy (przeniesienie funkcjonalności poza konstruktor)
void MainWindow::setConnections()
{
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(showOnStatusBar(int)) );                 // Qt->Qt
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setResolution(int)) );                   // Qt->VTK

    connect(this, SIGNAL(setStatusText(QString)),
            ui->statusBar, SLOT(showMessage(QString)) );        // Qt->Qt

    connect(ui->qvtkWidget, SIGNAL(mouseEvent(QMouseEvent*)),   //sygnał z widgetu, nie z VTK!
            this, SLOT(updateCoords()));

    // Zestawienie połączeń VTK->Qt:
    vtkSmartPointer<vtkEventQtSlotConnect> connections =
            vtkSmartPointer<vtkEventQtSlotConnect>::New();      // obiekt-pomost

    // przechwycenie zdarzeń vtk:
    connections->Connect(ui->qvtkWidget->GetInteractor(),       // pobranie obiektu (interaktora) z VTK
                            vtkCommand::LeftButtonPressEvent,   // wybranie sygnału
                            this,                               // ustalenie odbiorcy
                            SLOT(updateCoords()));              // ustalenie slotu odbiorcy, do którego trafi sygnał z VTK
                                                                // prototyp slotu odbiorcy:
                                                                //      SLOT( slotOdbiorcy( vtkObject*, unsigned long, void*, void*))

    //przykład podpięcia się pod postępy w obliczeniach, np. pod wczytywanie pliku:
    //    connections->Connect( XXXReader,
    //                          vtkCommand::ProgressEvent,
    //                          this,
    //                          SLOT( updateProgressValue( vtkObject*, unsigned long, void*, void*)));

    // debug poprawności połączeń na konsolę:
    connections->PrintSelf(cout, vtkIndent());  // tak można podglądać dowolne obiekty VTK!
}


MainWindow::~MainWindow()
{
    ren->Delete();

    delete ui;
}


// przykład automatycznego łączenia Sygnału ze Slotem
// poprzez odpowiednie "spreparowanie" nazwy Slotu
// taka nazwa "on_OBIEKT_SYGNAL" zastępuję metodę "connect()"
void MainWindow::on_actionZamknij_triggered()
{
    qApp->exit();
}
