#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <vector>
#include <QGroupBox>
#include "BitmapTO.h"


namespace Ui{
  class MainWindow;
}

/*!
  \brief Classe che rappresenta la GUI principale del sistema a partire
  dalla quale è possibile svolgere tutte le operazioni di gestione dell'immagine.
*/
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
/*
  Rappresentano le operazioni da effettuare in seguito all'attivazione
  di specifici eventi associati ad entità dell'interfaccia grafica.
*/
private slots:
  /*!
    \brief Permette l'apertura di un'immagine selezionabile dall'interfaccia grafica
  */
  void open();
  /*!
    \brief Mostra un panello informativo relativo al programma
  */
  void about();
  /*!
    \brief Mostra le informazioni specifiche relative all'immagine corrente
  */
  void infoImage();
  /*!
    \brief Modifica l'immagine corrente secondo l'effetto richiesto dall'utente
  */
  void modifyCurrentImage();
  /*!
    \brief Permette all'utente di salvare l'immagine corrente in una locazione del proprio sistema
  */
  void saveImage();

private:
  void createActions();
  void createMenus();
  void createImageGroupBox();
  void createModifyButtonBox();

  Ui::MainWindow *ui;
  QAction *openAct;
  QAction *exitAct;
  QAction *saveAct;
  QAction *aboutAct;
  QAction *infoImageAct;

  QMenu *fileMenu;
  QMenu *helpMenu;
  QGroupBox *imageGroup;
  QGroupBox *modifyGroup;
  BitmapTO* currentBitmap;

  QPushButton *modifyImage;
  std::vector<QCheckBox*> filterBoxes;
  QLabel *imageLabel;
  QScrollArea *scrollArea;
};

#endif // MAINWINDOW_H
