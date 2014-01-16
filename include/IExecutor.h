#ifndef IEXECUTOR_H
#define IEXECUTOR_H

#include "ResultStruct.h"
#include "RequestStruct.h"

/*!
  \brief Interfaccia per quelli che sono gli oggetti necessari allo svolgimento
  di uno specifico compito dettato dalla richiesta specificata.
*/
class IExecutor {
public:
  /*!
    \brief Esegue l'operazione necessaria per poter soddisfare la richiesta specificata e
    restituisce l'appropriato risultato per quest'ultima.

    @param request richiesta da dover soddisfare
    @return risultato prodotto dall'esecuzione dell'operazione
  */
  virtual ResultStruct executeOperation(RequestStruct request) = 0;
};

#endif // IEXECUTOR_H
