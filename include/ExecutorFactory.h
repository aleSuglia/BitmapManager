#ifndef EXECUTORFACTORY_H
#define EXECUTORFACTORY_H

#include "IExecutor.h"
#include "OpenImageExecutor.h"

/*!
  \brief Classe factory che provvede a generare istanze di oggetti Executor.

  La costruzione degli oggetti viene effettuata in modo opportuno analizzando
  quello che il contesto nel quale viene richiesto il servizio.
*/
class ExecutorFactory
{
private:
  ExecutorFactory();
public:
  /*!
    \brief Acquisisce il riferimento all'oggetto Executor che dovrà portare a termine la richiesta
    effettuata, esplicitata mediante il parametro di input.

    La richiesta specificata in input permette di estrapolare le informazioni inerenti
    alla tipologia di Executor da dover adoperare per portare a termine correntamente
    la richiesta.

    La struttura delle richieste pertanto deve essere precisamente formulata in modo tale da poter
    ottenere degli Executor validi.

    @param request richiesta da dover portare a termine
    @return Executor necessario per portare a termine la richiesta specificata
  */
  static IExecutor *getExecutor(RequestStruct &request);

};

#endif // EXECUTORFACTORY_H
