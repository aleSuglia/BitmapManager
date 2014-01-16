#ifndef MODIFYIMAGEEXECUTOR_H
#define MODIFYIMAGEEXECUTOR_H

#include "IExecutor.h"
#include "ResultStruct.h"
#include "RequestStruct.h"

/*!
  \brief Classe che implementa specifiche operazioni
  necessarie per la modifica di una determinata immagine.
*/
class ModifyImageExecutor : public IExecutor
{
public:
    /*!
      \brief Permette di applicare uno specifico effetto sull'immagine
      corrente.

      I parametri presenti all'interno della struttura di richiesta dovranno essere
      rigorosamente così disposti per poter far sì che l'operazione vada a buon fine:
        - operazione di modifica (ImageOperation)
        - effetto desiderato (ImageEffectOperation)
        - immagine attualmente caricata (BitmapTO)
      @param request richiesta correttamente strutturata per l'operazione da effettuare
      @return restituisce una struttura il cui contenuto presenta l'immagine correttamente modificata

      @throw std::logic_error nel caso in cui un effetto non valido venisse specificato
    */
    ResultStruct executeOperation(RequestStruct request);
};

/*!
  \brief Permette di applicare l'effetto seppia, con una intesità dell'effetto specificata in input,
  su quelli che sono i pixel dell'immagine specificati in input.

  E' stato verificato sperimentalmente che il valore 20 scelto come
  intensità dell'effetto risulta essere appropriato per ottenere un risultato opportuno.

  Ogni elemento della tripletta RGB viene codificato come carattere, pertanto
  la struttura del vettore di input sarà del tipo: [R,G,B,R,G,B,R,G,B]

  @param pixels sequenza di triplette RGB che rappresentano l'immagine
  @param sepiaDepth intesità dell'effetto da applicare

  @return sequenza di caratteri che rappresentano le triplette RGB modificate
*/
std::vector<unsigned char> imageToSepia(const std::vector<unsigned char>& pixels, int sepiaDepth);

/*!
  \brief Permette di applicare l'effetto negativo su quelli che sono i pixel dell'immagine
  specificati in input.


  Ogni elemento della tripletta RGB viene codificato come carattere, pertanto
  la struttura del vettore di input sarà del tipo: [R,G,B,R,G,B,R,G,B]

  @param pixels sequenza di triplette RGB che rappresentano l'immagine da modificare
  @return sequenza di triplette RGB modificate mediante l'effetto negativo
*/
std::vector<unsigned char> imageToNegative(const std::vector<unsigned char>& pixels);



#endif // MODIFYIMAGEEXECUTOR_H
