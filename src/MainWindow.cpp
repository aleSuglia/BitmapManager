#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "BitmapController.h"
#include "ImageOperation.h"
#include "BitmapFile.h"
#include "BitmapTO.h"
#include <string>
#include <QDir>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  QWidget* central_widget = new QWidget();
  QHBoxLayout* mainLayout = new QHBoxLayout();
  currentBitmap = NULL;

  createImageGroupBox();
  createModifyButtonBox();
  createActions();
  createMenus();

  mainLayout->addWidget(imageGroup);
  mainLayout->addWidget(modifyGroup);

  central_widget->setLayout(mainLayout);
  setCentralWidget(central_widget);
  resize(800, 600);
  setWindowTitle(tr("Bitmap Manager"));

}

MainWindow::~MainWindow()
{
  if(currentBitmap != NULL)
    delete currentBitmap;
  delete ui;
}


/* * SLOTS * */
void MainWindow::open() {
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Apri immagine"), QDir::homePath(),
                                                  tr("File immagine supportati (*.bmp)"));
  if (!fileName.isEmpty()) {

    try{

      ImageOperation op = OPEN_IMAGE;
      QByteArray ba = fileName.toLatin1();
      const char *str = ba.data();
      RequestStruct req(2,&op,str);

      ResultStruct result = BitmapController::executeOperation(req);
      // dealloca il contenuto della bitmap caricata in precedenza
      if(currentBitmap != NULL){
        delete currentBitmap;
      }

      // nuova bitmap allocata
      currentBitmap = (BitmapTO*)result.getAttribute(0);

      // Mostra a video l'immagina appena caricata
      QPixmap pixmap = QPixmap::fromImage(
            QImage(
              currentBitmap->getPixelDataPtr(),
              currentBitmap->getWidth(),
              currentBitmap->getHeight(),
              QImage::Format_RGB888
              )
            );


      imageLabel->setPixmap(pixmap);
      imageLabel->adjustSize();


    } catch(std::logic_error& e) {
      std::string errorMsg = std::string("Impossibile effettuare il rendering del file scelto: \n") + std::string(e.what());
      QMessageBox::critical(this, tr("ATTENZIONE"), errorMsg.c_str());
    }
  }

}

void MainWindow::modifyCurrentImage() {
  if(imageLabel->pixmap() == NULL) {
    QMessageBox::critical(this, tr("ATTENZIONE"), tr("Non è stata caricata alcuna immagine. Impossibile apportare modifiche"));
    return;
  }

  std::string selectedEffect;
  for(std::vector<QCheckBox*>::const_iterator it = filterBoxes.begin(),
      ed = filterBoxes.end();
      it != ed;
      ++it) {
    QCheckBox* box = *it;

    if(box->isChecked()) {
      selectedEffect = box->text().toStdString();
    }
  }

  try{
    ImageOperation imageOp = MODIFY_IMAGE;
    ImageEffectOperation effectOp = getEffectEnumValue(selectedEffect);

    ResultStruct result = BitmapController::executeOperation(RequestStruct(3, &imageOp, &effectOp, currentBitmap));

    currentBitmap = (BitmapTO*)result.getAttribute(0);
    QPixmap pixmap = QPixmap::fromImage(
          QImage(
            currentBitmap->getPixelDataPtr() ,
            currentBitmap->getWidth(),
            currentBitmap->getHeight(),
            QImage::Format_RGB888
            )
          );
    imageLabel->setPixmap(pixmap);
    imageLabel->adjustSize();

    QMessageBox::information(this, tr("Operazione completata"),
                             tr("L'operazione di modifica dell'immagine scelta è stata completata con successo"));
  } catch(std::invalid_argument& e) {
    QMessageBox::critical(this, tr("ATTENZIONE"), tr(e.what()));

  }

}

void MainWindow::saveImage() {
  if(imageLabel->pixmap() == NULL) {
    QMessageBox::critical(this, tr("ATTENZIONE"), tr("Non è stata caricata alcuna immagine. Impossibile effettuare il salvataggio"));
    return;
  }


  QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Salva immagine"),
        QDir::homePath(),
        tr("Formati d'immagine supportati' (*.bmp)") );

  if(!fileName.isEmpty()) {

    ImageOperation op = SAVE_IMAGE;
    QByteArray ba = fileName.toLatin1();
    const char *str = ba.data();
    try{
      ResultStruct result = BitmapController::executeOperation(RequestStruct(3, &op,currentBitmap, str));
      char* msg = (char*) result.getAttribute(0);
      QMessageBox::information(this, tr("Operazione completata"), tr(msg));
    } catch( std::logic_error& e) {
      std::string errorMsg = std::string("Impossibile salvare l'immagine: \n") + std::string(e.what());
      QMessageBox::critical(this, tr("ATTENZIONE"), errorMsg.c_str());

    }
  }

}

void MainWindow::about() {
  QMessageBox::about(this, tr("About"),
                     tr("<img src=\"icon.png\" width=\"32\" height=\"32\">"
                       "<p>Questo programma è stato realizzato a scopo didattico "
                        " durante l'insegnamento di Sistemi Multimediali per poter approfondire gli aspetti riguardanti la gestione di immagini "
                        "ed in particolare per poter studiare in modo approfondito il formato "
                        "<a href=http://en.wikipedia.org/wiki/BMP_file_format>BITMAP. </a></p>"
                        "<p>Mediante l'utilizzo di tale sistema sarà possibile effettuare una serie di operazioni, quali:</p>"
                        "<ul><li>Aprire un file .bmp</li> "
                        "<li>Salvare un'immagine appena caricata in una specifica locazione del sistema</li>"
                        "<li>Modificare l'immagine caricata applicando varie tipologie di effetti</li></ul>"
                        "<br><br>"
                        "Il sistema sviluppato è stato realizzato da Alessandro Suglia ed "
                        "è reperibile al seguente indirizzo: <a href=https://github.com/aleSuglia/BitmapManager>"
                        "https://github.com/aleSuglia/BitmapManager</a>"
                        "<br><br>"
                        "<p align=center>Alessandro Suglia - alessandro.suglia@gmail.com (2014) </p>"

                        ));

}

void MainWindow::infoImage() {
  if(currentBitmap == 0)
    QMessageBox::critical(this, tr("ATTENZIONE"), tr("Nessuna immagine caricata.\n"
                                                     "Impossibile visualizzare le informazioni del file."));
  else {

    QMessageBox msg(QMessageBox::Information, tr("Proprietà immagine"), tr(currentBitmap->getInfoBitmap()));
    msg.setMinimumWidth(100);
    msg.exec();

  }
}

void MainWindow::createMenus() {
  fileMenu = new QMenu(tr("&File"), this);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(infoImageAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  helpMenu = new QMenu(tr("&Info"), this);
  helpMenu->addAction(aboutAct);

  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(helpMenu);
}

void MainWindow::createActions() {

  openAct = new QAction(tr("&Apri..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct = new QAction(tr("&Salva..."), this);
  saveAct->setShortcut(tr("Ctrl+S"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(saveImage()));

  infoImageAct = new QAction(tr("&Proprietà"), this);
  infoImageAct->setShortcut(tr("Ctrl+P"));
  connect(infoImageAct, SIGNAL(triggered()), this, SLOT(infoImage()));

  exitAct = new QAction(tr("&Esci"), this);
  exitAct->setShortcut(tr("Ctrl+E"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setShortcut(tr("Ctrl+A"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

}

void MainWindow::createImageGroupBox() {
  imageGroup = new QGroupBox(tr("Immagine caricata"));
  QVBoxLayout* vBox = new QVBoxLayout();

  imageLabel = new QLabel;
  imageLabel->setBackgroundRole(QPalette::Base);
  imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  imageLabel->setScaledContents(true);

  scrollArea = new QScrollArea;
  scrollArea->setBackgroundRole(QPalette::Dark);
  scrollArea->setWidget(imageLabel);

  vBox->addWidget(scrollArea);

  imageGroup->setLayout(vBox);

}
void MainWindow::createModifyButtonBox() {
  modifyGroup = new QGroupBox(tr("Opzioni modifica immagine"));

  QVBoxLayout* vBox = new QVBoxLayout();

  QButtonGroup* group = new QButtonGroup();

  std::vector<std::string> filterNames = getSupportedFilters();
  QCheckBox *box;

  for(std::vector<std::string>::const_iterator it = filterNames.begin(),
      ed = filterNames.end();
      it != ed;
      ++it) {
    box = new QCheckBox(tr(it->c_str()));
    filterBoxes.push_back(box);
    group->addButton(box);
    vBox->addWidget(box);
  }

  modifyImage = new QPushButton(tr("Modifica immagine"));

  connect(modifyImage, SIGNAL(clicked()),this, SLOT(modifyCurrentImage()));

  vBox->addWidget(modifyImage);

  vBox->addStretch(10);
  modifyGroup->setLayout(vBox);


}

