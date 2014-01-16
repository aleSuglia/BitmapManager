#ifndef OPENIMAGEEXECUTOR_H
#define OPENIMAGEEXECUTOR_H

#include "IExecutor.h"

/*!
  \brief Classe che ha il compito di consentire l'apertura di uno specifico
  file immagine indicato dall'utente.
*/
class OpenImageExecutor : public IExecutor
{
public:
  /*!
    \brief Esegue le operazioni necessarie per garantire l'apertura dell'immagine
    richiesta dall'utente.

    La struttura specificata in input deve possedere una struttura ben precisa per
    permettere un corretto svolgimento delle operazioni.
    Questa dovrà essere organizzata nel seguente modo:
      - operazione di apertura di un'immagine (ImageOperation)
      - path dell'immagine da aprire (std::string)
    @param request richiesta di apertura di una determinata immagine
    @return l'immagine correttamente acquisita (BitmapTO)

    @throw std::logic_error nel caso in cui un errore nell'apertura dell'immagine venisse riscontrato
  */
  ResultStruct executeOperation(RequestStruct request);
};

#endif // OPENIMAGEEXECUTOR_H
