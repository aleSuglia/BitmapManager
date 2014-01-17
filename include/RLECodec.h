#ifndef RLECODEC_H
#define RLECODEC_H

#include <vector>

/*!
  \brief Tipo enumerativo che elenca le possibili modalità di compressione supportate
*/
typedef enum {
  BI_RGB, ///<	Formato non compresso
  BI_RLE8, ///< Algoritmo RLE per immagini bitmap a 8 bit
  BI_RLE4 ///<	Algoritmo RLE per immagini bitmap a 4 bit
} BmpCompression;

/*!
  \brief Classe che implementa una serie di funzionalità per mettere in pratica la compressione secondo
  l'algoritmo RLE (applicato alle immagini BITMAP - <a href="http://msdn.microsoft.com/en-us/library/cc250386.aspx">documentazione RLE</a>)

  Rispetto a quelle che sono le modalità di compressione supportate (BmpCompression) procede a comprimere
  o decomprimere l'insieme di byte passato in input e a restituire il risultato dell'operazione richiesta.
*/
class RLECodec
{
public:
  /*!
    \brief Decodifica il contenuto del vettore passato in input secondo l'algoritmo di compressione specificato

    Nel caso in cui non dovesse essere passato un valore adeguato come secondo parametro, verrebbe restituito
    un vettore privo di elementi.

    @param vet vettore di caratteri da decomprimere
    @param compr_mode algoritmo di compressione da utilizzare (BmpCompression)
    @param width larghezza immagine
    @param height altezza immagine

    @return vettore di caratteri opportunamente decompresso
  */
  static std::vector<unsigned char> decode_array(const std::vector<unsigned char>& vet, BmpCompression compr_mode, int width, int height);
  /*!
    \brief Codifica il contenuto del vettore passato in input secondo l'algoritmo di compressione specificato

    Il contenuto del vettore deve essere strutturato esattamente rispetto ai canoni del formato BITMAP relativi ad un'immagine
    a 8-bit per pixel o 4-bit per pixel.

    Nel caso in cui non dovesse essere passato un valore adeguato come secondo parametro, verrebbe restituito
    un vettore privo di elementi.

    @param vet vettore di caratteri da comprimere
    @param compr_mode algoritmo di compressione da utilizzare (BmpCompression)
    @param width larghezza dell'immagine
    @param height altezza dell'immagine

    @return vettore di caratteri opportunamente compresso
  */
  static std::vector<unsigned char> encode_array(const std::vector<unsigned char>& vet, BmpCompression compr_mode, int width, int height);

private:
  static std::vector<unsigned char> decode_rle_8(const std::vector<unsigned char>& vet, unsigned int width, unsigned int height);
  static std::vector<unsigned char> decode_rle_4(const std::vector<unsigned char>& vet, unsigned int width, unsigned int height);

  static std::vector<unsigned char> encode_rle_8(const std::vector<unsigned char>& vet, int width, int height);
  static std::vector<unsigned char> encode_rle_4(const std::vector<unsigned char>& vet, int width, int height);
};

std::vector<unsigned char> generate_vet_nibble(std::vector<unsigned char>::const_iterator start, std::vector<unsigned char>::const_iterator end_it);

#endif // RLECODEC_H
