#ifndef REQUESTSTRUCT_H
#define REQUESTSTRUCT_H

#include <vector>

/*!
  \brief Classe che rappresenta la struttura per poter effettuare
  richiesta di una specifica operazione
*/
class RequestStruct
{
private:
  RequestStruct();
public:
  /*!
    \brief Permette di creare una struttura che accoglierà uno specifico
    insieme di valori, il cui numero viene specificato in input

    E' possibile accogliere qualsiasi tipo di dato purchè esso venga specificato
    come puntatore.
    Obbligatoriamente è necessario specificare un numero di parametri per la struttura
    pari a quello indicata dal primo parametro.

    @param num_args numero di parametri che si sta specificando
    @param varargs argomenti di input variabili

  */
  RequestStruct(int num_args, ...);
  RequestStruct(const RequestStruct& request);
  RequestStruct& operator=(const RequestStruct& request);

  /*!
    \brief Restituisce l'attributo della struttura presente nella posizione
    specificata dal valore in input

    L'indice rappresenta la posizione dell'attributo nel momento in cui è stata
    creata la struttura.

    @param index posizione dell'attributo nella struttura
    @return riferimento all'oggetto richiesto
  */
  void *getAttribute(int index);
private:
  std::vector<void*> var_list; ///< lista di entità da memorizzare
};

#endif // REQUESTSTRUCT_H
