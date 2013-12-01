#ifndef BITMAPCONTROLLER_H
#define BITMAPCONTROLLER_H

#include "ResultStruct.h"
#include "RequestStruct.h"

class BitmapController {
public:
  /*
    Elabora la richiesta proveniente dalle view e contenuta nella
    struttura di input e restituisce una struttura che presenta
    il risultato prodotto a seguito dello svolgimento delle operazioni
    necessarie per poter soddisfare la richiesta
    */
  static ResultStruct executeOperation(RequestStruct request);

};

#endif // BITMAPCONTROLLER_H
