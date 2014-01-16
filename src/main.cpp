#include "MainWindow.h"
#include "BitmapFile.h"
#include <QApplication>

/*! \mainpage BITMAP Manager
 *
 * \section intro_sec Introduzione
 *
 * Assieme a quello che è il sistema operativo maggiormente adoperato al mondo, Microsoft Windows, veniva parallelamente sviluppato un formato di file
 * per garantire la memorizzazione dell'immagini necessarie al sistema stesso per poter dare la possibilità
 * all'utente di personalizzare a proprio piacimento il comparto grafico del sistema.
 * Da tali presupposti e da tali necessità venne creato, in collaborazione tra Microsoft e IBM, uno dei primi
 * formati d'immagine incredibilmente adoperati negli anni d'oro dello sviluppo dei personal computer, il formato di file immagine BITMAP.
 * Questo risulta essere adottato ancora adesso per rappresentare quasi tutte quelle che sono le immagini e gli elementi grafici dei quali
 * si compone un qualsiasi sistema operativo realizzato da Microsoft.
 *
 * Il sistema implementato si prefigge di supportare al meglio la struttura del formato, in particolare in
 * questo lavoro si farà riferimento alla versione 3 del formato BITMAP.
 * Pertanto sarà possibile aprire e modificare UNICAMENTE immagini con profondità di colore pari a 1, 4, 8 e 24 ed
 * inoltre viene parzialmente supportata la lettura di immagini codificata con RLE-4 o RLE-8. Il salvataggio di queste
 * due tipologie di immagine è sconsigliato in quanto non consente l'applicazione della compressione sul nuovo file.
 *
 */
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
