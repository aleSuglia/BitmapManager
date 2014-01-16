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
    int getWidth() const;
    void setWidth(int value);
    int getHeight() const;
    void setHeight(int value);
    std::vector<unsigned char> getPixelData() const;
    unsigned char* getPixelDataPtr();
    void setPixelData(const std::vector<unsigned char> &value);
    int getBitsPerPixel() const;
    void setBitsPerPixel(int value);
    int getBitmapSize() const;
    void setBitmapSize(int value);
    const char *getInfoBitmap() const;
    void setInfoBitmap(const std::string &value);
    int getComprMode() const;
    void setComprMode(int value);
};

#endif // BITMAPTO_H
