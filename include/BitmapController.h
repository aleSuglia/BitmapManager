#ifndef BITMAPCONTROLLER_H
#define BITMAPCONTROLLER_H

#include "ResultStruct.h"
#include "RequestStruct.h"

/*!
 \brief
  Classe necessaria per veicolare i dati all'interno dell'applicazione
  fra i vari livelli.

  Gestisce le richieste provenienti dai componenti che si occupano dell'interfaccia
  grafica ed elabora le richieste di quest'ultime al fine di aumentare la
  modularità del sistema.
*/
class BitmapController {
public:
  /*!
    \brief Esegue un'elaborazione sulla struttura di input producendo una struttura di risposta

    Elabora la richiesta proveniente dalle view e contenuta nella
    struttura di input e restituisce una struttura che presenta
    il risultato prodotto a seguito dello svolgimento delle operazioni
    necessarie per poter soddisfare la richiesta

    @param request contesto nel quale deve essere svolta l'operazione
    @return risultato dell'operazione svolta

  */
  static ResultStruct executeOperation(RequestStruct request);

};

#endif // BITMAPCONTROLLER_H
