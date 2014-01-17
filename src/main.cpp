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
 * inoltre viene parzialmente supportata la lettura e scrittura di immagini codificate con RLE-4 o RLE-8.
 *
 * \section install_sec Requisiti
 *  L'intero sistema è stato sviluppato adoperando le librerie QT in versione 5.1.1 e come toolchain di compilazione
 *  MinGW a 64 bit sul sistema operativo Windows 8.
 *
 *  \section license_sec Licenza
 *  Tale sistema è stato sviluppato per fini didattici pertanto su di esso non è previsto alcun tipo di brevetto e/o proprietà.
 *  Si auspica che, nel caso in cui venga adoperato per ulteriori sviluppi, venga resa nota la provenienza del codice.
 *  Per maggiori informazioni in merito alla licenza del codice si faccia riferimento alla GNU GPL v3 che è possibile trovare
 *  al sequente indirizzo: <a href="http://www.gnu.org/licenses/gpl-3.0.txt"> GNU GPL </a>
 *
 */
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
