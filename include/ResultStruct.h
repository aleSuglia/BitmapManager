#ifndef RESULTSTRUCT_H
#define RESULTSTRUCT_H
#include <vector>
#include <cstdarg>

/*!
  \brief Classe che rappresenta la struttura per poter memorizzare
  il risultato generato a seguito dell'esecuzione di una data operazione
*/
class ResultStruct
{
public:
  /*!
    \brief Permette di creare una struttura che accoglierà uno specifico
    insieme di valori, il cui numero viene specificato in input

    E' possibile accogliere qualsiasi tipo di dato purchè esso venga specificato
    come puntatore.
    Obbligatoriamente è necessario specificare un numero di parametri per la struttura
    pari a quello indicata dal primo parametro.

    @param num_args numero di parametri che si sta specificando
    @param ... argomenti di input variabili

  */
  ResultStruct(int num_args, ...);
  /*!
    \brief Costruisce una struttura priva di alcun attributo
  */
  ResultStruct();
  ResultStruct(const ResultStruct& result);
  ResultStruct& operator=(const ResultStruct& result);

  /*!
    \brief Restituisce l'attributo della struttura presente nella posizione
    specificata dal valore in input

    L'indice rappresenta la posizione dell'attributo nel momento in cui è stata
    creata la struttura.

    @param index posizione dell'attributo nella struttura
    @return riferimento all'oggetto richiesto
  */
  void* getAttribute(int index);
private:
  std::vector<void*> var_list; ///< lista di entità da memorizzare
};

#endif // RESULTSTRUCT_H
