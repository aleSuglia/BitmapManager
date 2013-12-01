#include "mainwindow.h"
#include "BitmapFile.h"
#include <QApplication>
#include <QtWidgets>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
//    BitmapFile file("C:\\Users\\AlexZ\\Desktop\\Sistemi multimediali\\samples\\pal1.bmp");

//    //file.copy_bmp("new.bmp");

    return 0;

}
