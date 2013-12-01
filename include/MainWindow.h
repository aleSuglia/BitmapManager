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
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void open();
  void about();
  void zoomIn();
  void zoomOut();
  void modifyCurrentImage();
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
  QAction *zoomInAct;
  QAction *zoomOutAct;
  QAction *aboutAct;

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
