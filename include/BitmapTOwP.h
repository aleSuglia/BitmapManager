#ifndef BITMAPTOWP_H
#define BITMAPTOWP_H

#include "BitmapTO.h"

/*!
  \brief Classe che rappresenta un trasfer object per immagini aventi una palette.

  Tale classe estende la classe BitmapTO e aggiunge a quest'ultima la palette, elemento
  fondamentale per quelle che sono le immagini con profondità di colore pari a 1, 4 e 8 bit.
*/
class BitmapTOwP : public BitmapTO
{
private:
  unsigned char **palette; ///< palette di colori adoperata dall'immagine
public:
  /*!
    \brief Costruisce un transfer object vuoto.

    Questo costruttore può essere adoperato nel caso in cui
    sia necessario costruire l'oggetto adoperando i metodi set
    messi a disposizione dall'interfaccia di tale classe.
  */
  BitmapTOwP();
  ~BitmapTOwP();
  /*!
    \brief Costruisce un transfer object a partire da quelle che sono le informazioni specificate.

    Una volta adoperato tale costruttore, il transfer object potrà essere direttamente adoperato
    in quanto verranno opportunamente inizializzati tutte le sue componenti.

    @param vet vettore di caratteri che rappresenta il contenuto dell'immagine come una serie di triplette RGB
    @param width larghezza dell'immagine
    @param height altezza dell'immagine
    @param bitmap_size dimensione effettiva della sezione dati dell'immagine
    @param num_bits profondità di colore dell'immagine
    @param palette palette dei colori adoperata dall'immagine
    @param info_bitmap rappresentazione testuale formattata dei metadati dell'immagine
    @param compr_mode algoritmo di compressione applicato all'immagine

  */
  BitmapTOwP(std::vector<unsigned char> vet, int width, int height, int bitmap_size, int num_bits, unsigned char *palette[4], std::string info_bitmap, int compr_mode);
  BitmapTOwP(const BitmapTOwP& to);
  BitmapTOwP& operator=(const BitmapTOwP& to);
  /*!
    \brief Restituisce la palette di colori dell'immagine corrente

    La dimensione complessiva di quest'ultima può essere facilmente ricavata
    adoperando la profondità di colore ed applicando la seguente formula:
    dimensione_palette = 2^bit_depth
    dove bit_depth è la profondità di colore dell'immagine

    @return palette di colori dell'immagine
  */
  unsigned char **getPalette() const;

  /*!
    \brief Modifica la palette di colori dell'immagine corrente con  quella specificata

    Nessun controllo viene effettuato sul valore passato in input, in quanto la palette
    specificata in input non viene completamente copiata dalla funzione.
    E' pertanto necessario che il riferimento resti valido per l'intero tempo d'utilizzo
    dell'oggetto corrente.

    @param value nuova palette di colori per l'immagine corrente
  */
  void setPalette(unsigned char **value);
};

#endif // BITMAPTOWP_H
