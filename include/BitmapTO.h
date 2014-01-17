#ifndef BITMAPTO_H
#define BITMAPTO_H

#include <vector>
#include <string>


/*!
  \brief Classe che rappresenta un transfer object per oggetti di tipo
  BitmapFile.

  Tale classe contiene tutti gli elementi necessari per poter rappresentare
  l'immagine che il sistema ha correntemente caricato in memoria, senza dover
  considerare i dettagli di basso livello, ma presentando unicamente i dati
  necessari alle view per poter effettuare operazioni sull'immagine.
*/
class BitmapTO
{
protected:
    std::vector<unsigned char> pixelData; ///< rappresentazione in byte dell'immagine
    unsigned char* aligned_data; ///< rappresentazione in byte dell'immagine con le righe allineate a 32-bit
    int width; ///< larghezza dell'immagine corrente
    int height; ///< altezza dell'immagine corrente
    int bitmap_size; ///< numero di byte che rappresentano i pixel dell'immagine
    int bits_per_pixel; ///< profondità di colore dell'immagine
    std::string info_bitmap; ///< rappresentazione testuale dei metadati dell'immagine
    int compr_mode; ///< modalità di compressione adoperata per l'immagine corrente

    /*!
      \brief Avvalora il campo aligned_data producendo una forma allineata 32-bit dei byte dell'immagine

      Tale forma rappresentativa è necessaria per poter lavorare con immagini la cui larghezza
      non risulti essere multiplo di 4.
      In tale situazione risulta essere necessario introdurre dei byte di padding per ogni
      riga dell'immagine garantendo un perfetto allineamento.
    */
    void prepare_aligned_data();
public:
    /*!
      \brief Costruisce un oggetto senza alcun informazione all'interno
    */
    BitmapTO();
    ~BitmapTO();
    /*!
      \brief Costruisce un transfer object sfruttando i parametri passati in input.

      L'oggetto costruito possederà come rappresentazione in byte dell'immagine di larghezza width
      e altezza height, il vettore vec di dimensione pari a bitmap_size.
      La profondità di colore dell'immagine è rappresentata dal parametro num_bits e altre informazioni
      sull'immagine corrente risultano essere presenti all'interno della stringa info_bitmap.

      @param vec rappresentazione in byte dell'immagine (in forma RGB)
      @param width larghezza dell'immagine corrente
      @param height altezza dell'immagine corrente
      @param bitmap_size dimensione della parte dati dell'immagine (grandezza del vettore di byte)
      @param num_bits profondità di colore dell'immagine
      @param info_bitmap rappresentazione testuale dei metadati dell'immagine
      @param compr_mode algoritmo di compressione applicato sull'immagine
    */
    BitmapTO(std::vector<unsigned char> vec, int width, int height, int bitmap_size,int num_bits, std::string info_bitmap, int compr_mode);
    BitmapTO(const BitmapTO& to);
    BitmapTO& operator=(const BitmapTO& to);
    /*!
      \brief Restituisce la larghezza dell'immagine corrente

      @return la larghezza dell'immagine
    */
    int getWidth() const;
    /*!
      \brief Modifica la larghezza corrente dell'immagine con il valore specificato

      @param value nuovo valore per la larghezza dell'immagine
    */
    void setWidth(int value);
    /*!
      \brief Restituisce l'altezza dell'immagine

      @return altezza dell'immagine corrente
    */
    int getHeight() const;
    /*!
      \brief Modifica l'altezza dell'immagine con il valore specificato
    */
    void setHeight(int value);
    /*!
      \brief Restituisce la struttura dati adoperata per memorizzare i pixel dell'immagine

      E' opportuno evitare di restituire frequentemente il suddetto oggetto in quanto
      viene effettuata, ad ogni chiamata, una copia della struttura, la quale potrebbe contenere
      un numero davvero elevato di elementi

      @return la struttura dati contenente i pixel dell'immagine nella forma RGB
    */
    std::vector<unsigned char> getPixelData() const;
    /*!
      \brief Restituisce un riferimento alla struttura dati adoperata per memorizzare i pixel dell'immagine

      @return riferimento alla struttura dati contenente i pixel dell'immagine nella forma RGB
    */
    unsigned char* getPixelDataPtr();
    /*!
      \brief Modifica il contenuto dell'immagine con quello indicato dalla struttura passata in input
      @param value struttura dati contenente il nuovo contenuto dell'immagine

    */
    void setPixelData(const std::vector<unsigned char> &value);
    /*!
      \brief Restituisce la profondità di colore dell'immagine
      @return profondità di colore rappresentata come un intero
    */
    int getBitsPerPixel() const;
    /*!
      \brief Modifica il valore corrente della profondità di colore con quello specificato
    */
    void setBitsPerPixel(int value);
    /*!
      \brief Restituisce l'occupazione di memoria dell'immagine

      Tale misura viene fornita in byte e non rappresenta il numero di elementi presenti
      all'interno della struttura dati contenente i pixel, bensì rappresenta il numero di
      elementi necessari per rappresentare l'immagine sul disco

      @param value nuovo valore di profondità di colore dell'immagine
    */
    int getBitmapSize() const;
    /*!
      \brief Modifica la dimensione della sezione dati dell'immagine con quella specificata
      @param value nuova dimensione della sezione dati
    */
    void setBitmapSize(int value);
    /*!
      \brief Restituisce i metadati dell'immagine in forma testuale

      @return rappresentazione testuale formattata dei campi dell'header dell'immagine
    */
    const char *getInfoBitmap() const;
    /*!
      \brief Modifica il valore relativo alla rappresentazione dei metadati dell'immagine
      con quello specificato in input

      @param value nuova rappresentazione testuale dei metadati dell'immagine
    */
    void setInfoBitmap(const std::string &value);
    /*!
     * \brief Restituisce un valore simbolico che rappresenta la modalità di compressione adottata per l'immagine
     *
     *  I valori che la funzione può restituire sono uguali a quelle del tipo enumerativo BmpCompression
     *
     * @return valore simbolico che rappresenta l'algoritmo di compressione adoperato per l'immagine corrente
     */
    int getComprMode() const;
    /*!
      \brief Modifica il valore simbolico che rappresenta l'algoritmo di compressione adottato

      @param value nuovo valore simbolico che rappresenta la tecnica di compressione
    */
    void setComprMode(int value);
};

#endif // BITMAPTO_H
