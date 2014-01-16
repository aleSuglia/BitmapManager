#ifndef SAVEIMAGEEXECUTOR_H
#define SAVEIMAGEEXECUTOR_H

#include "IExecutor.h"
#include "ResultStruct.h"
#include "RequestStruct.h"


/*!
  \brief Classe che provvede ad effettuare l'operazione di salvataggio
  di una data immagine sul disco.
*/
class SaveImageExecutor : public IExecutor
{
public:
  /*!
    \brief Esegue l'operazione di salvataggio dell'immagine adoperando
    i parametri specificati nella struttura di richiesta.

    Per portare a termine l'operazione in modo corretto, è necessario
    che la richiesta venga formulata in modo opportuno, ovvero, gli
    attributi da dover specificare devono presentare la seguente forma:
      - operazione di salvataggio (ImageOperation)
      - immagine da salvare (BitmapTO)
      - nome del file nel quale salvarla (std::string)
  */
  ResultStruct executeOperation(RequestStruct request);
};

#endif // SAVEIMAGEEXECUTOR_H
