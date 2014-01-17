#ifndef BITMAPFILE_H
#define BITMAPFILE_H

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include "RLECodec.h"
#include "BitmapTOwP.h"
#include "PaletteIndexer.h"

/*!
  \brief Struttura del FILE header.

  Contiene tutte le informazioni relative alla tipologia di file
  e alla sua occupazione di memoria.
*/
typedef struct {
  char         file_type[2]; ///< Sigla che rappresenta il tipo di file corrente (comunemente uguale a BM)
  unsigned int file_size; ///< dimensione totale del file
  short        reserved1; ///< valore riservato fissato a 0
  short        reserved2; ///< valore riservato fissato a 0
  unsigned int data_offset; ///< offset necessario per indirizzare il blocco relativo ai dati dell'immagine
} FileHeader;


/*!
  \brief Struttura del DIB Header

  Contiene tutti i componenti specifici di un'immagine BITMAP
  necessari per la sua elaborazione e gestione.
*/
typedef struct {
  unsigned int header_size; ///< dimensione dell'header corrente
  int          width; ///< larghezza dell'immagine
  int          height; ///< altezza dell'immagine
  short        planes; ///< numero di piani di colore adoperati
  short        bits_per_pixel;  ///< profondità del colore (supportate immagini a 24-bit)
  unsigned int compression; ///< compressione non supportata
  int 		 bitmap_size; ///< dimensione della parte relativa ai dati (pixel dell'immagine)
  int          horizontal_res; ///< risoluzione orizzontale dell'immagine
  int          vertical_res; ///< risoluzione verticale dell'immagine
  unsigned int num_colors; ///< numero di colori nella palette adoperata
  unsigned int important_colors; ///< numero di colori rilevanti (0 se sono tutti importanti)
  unsigned char **palette; ///< palette di colori adoperata dall'immagine (settato unicamente se bit-depth <= 8)

} DibHeader;

/*!
  \brief Classe principale per la gestione di file BITMAP aventi come struttura
  quella dell'header v3.

  Questo, di fatto, prevede quattro sezioni principali tra le quali:
    - FILE Header
    - DIB Header
    - Palette (opzionale)
    - Pixel array

*/
class BitmapFile {
private:
  FileHeader file_header; ///< file header di questa immagine
  DibHeader dib_header; ///< dib header di questa immagine
  std::vector<unsigned char> reversed_pixel_data; ///< rappresentazione RGB dell'immagine
  std::vector<unsigned char> original_pixel_data; ///< contenuto dell'immagine originale

public:
  /**
    \brief Costruisce un BitmapFile a partire da uno specifico
    file BITMAP il cui percorso viene specificato come parametro.
    @param file_name percorso di un file di tipo BITMAP da leggere
  */
  BitmapFile(const std::string& file_name);
  /**
    Costruisce un BitmapFile a partire da un opportuno transfer object.
    @see BitmapTO
  */
  BitmapFile(BitmapTO& to);
  BitmapFile(const BitmapFile& bmp);
  BitmapFile& operator=(const BitmapFile& bmp);
  ~BitmapFile();
  /**
    \brief Restituisce la rappresentazione primitiva del contenuto dell'immagine
    Il suo contenuto rappresenta l'immagine BITMAP corrente, le cui righe sono
    allineate a 32 bit (vengono introdotti byte di padding).
  */
  unsigned char* get_data_array();
  /**
    \brief Restituisce il numero di byte che rappresentano i pixel dell'immagine
    @return un valore intero rappresentante la dimensione in byte dell'array di pixel
  */
  int bitmap_size();
  /**
    @return l'altezza dell'immagine corrente
  */
  int height_size();
  /**
    @return la larghezza dell'immagine corrente
  */
  int width_size();
  /**
    \brief Copia l'immagine BITMAP corrente in un nuovo file
    @param file_name path del file che verrà creato per accogliere la copia dell'immagine.
  */
  void copy_bmp(const std::string& file_name);
  /**
    \brief Crea un transfer object per l'immagine corrente
    Viene generato un transfer object opportuno a partire dall'immagine corrente.
    Se non dovesse essere presente alcun valore all'interno dell'array di pixel
    allora tale metodo restituisce 0.
    @return l'indirizzo dell'oggetto BitmapTO creato (0 se original_pixel_array.empty() == true)
  */
  BitmapTO* getTO();

private:
  // debug_mode
  void print_bitmap_information();
  /**
    \brief Provvede a leggere il contenuto del file specificato e avvalora l'immagine corrente
  */
  void read_bmp_file(const std::string& file_name);
  /**
    \brief Riproduce l'array di byte da adoperare per la memorizzazione a partire da un'immagine con
    profondità di colore pari a 1.

    @param palette_index palette necessaria per poter riprodurre i colori secondo il formato dell'immagine
    @param temp_array rappresenta l'immagine come un insieme di caratteri disposti nel formato BGR

  */
  void generate_pixel_array_1(const PaletteIndexer& palette_index, const std::vector<unsigned char>& temp_array);
  /**
    \brief Riproduce l'array di byte da adoperare per la memorizzazione a partire da un'immagine con
    profondità di colore pari a 4.

    @param palette_index palette necessaria per poter riprodurre i colori secondo il formato dell'immagine
    @param temp_array rappresenta l'immagine come un insieme di caratteri disposti nel formato BGR

  */
  void generate_pixel_array_4(const PaletteIndexer& palette_index, const std::vector<unsigned char>& temp_array);
  /**
    \brief Riproduce l'array di byte da adoperare per la memorizzazione a partire da un'immagine con
    profondità di colore pari a 8.

    @param palette_index palette necessaria per poter riprodurre i colori secondo il formato dell'immagine
    @param temp_array rappresenta l'immagine come un insieme di caratteri disposti nel formato BGR

  */
  void generate_pixel_array_8(const PaletteIndexer& palette_index, const std::vector<unsigned char>& temp_array);
  /**
    \brief Riproduce l'array di byte da adoperare per la memorizzazione a partire da un'immagine con
    profondità di colore pari a 24.
    Viene creato un array di caratteri che rappresenta l'immagine secondo lo standard imposto dal formato
    BITMAP.

  */
  void generate_pixel_array_24();
  /**
    \brief Restituisce i metadati dell'immagine opportunamente formattati
    @return Rappresentazione testuale formattata dai metadati dell'immagine
  */
  std::string getInfoBitmap() const;
  /**
    \brief Avvalora gli header dell'immagine corrente adoperando i parametri specificati in input

    Tutti i parametri passati in input vengono considerati, salvo la palette che risulta essere
    opzionale nel momento in cui si adopera un'immagine con profondità di colore pari a 24 e
    la modalità di compressione che normalmente non viene adoperata (valore di default 0).

    @param width larghezza dell'immagine corrente
    @param height altezza dell'immagine corrente
    @param bits_per_pixel profondità di colore dell'immagine
    @param bitmap_size numero di byte necessari per memorizzare i pixel dell'immagine
    @param palette di colori adoperata dall'immagine
    @param compr_mode algoritmo di compressione applicato
  */
  void fill_bmp_header(int width, int height, int bits_per_pixel, int bitmap_size, unsigned char **palette = 0, int compr_mode = 0);
  /**
    \brief Mostra sulla console il contenuto del vettore passato come parametro
    @param vet il vettore da dover mostrare a video
  */
  void print_pixel_data(const std::vector<unsigned char>& vet);
  /**
    \brief Crea la struttura originale dell'array di byte dell'immagine.
    La creazione dell'array di byte avviene adoperando il vettore di caratteri
    nel formato RGB che rappresenta l'immagine corrente
  */
  void retrieve_original_pixel_data();
  /**
    \brief Effettua dei controlli di consistenza dell'header dell'immagine corrente
  */
  bool is_dib_header_valid();
  /**
    \brief A partire dalla rappresentazione in byte memorizzata nel file
    produce un vettore di caratteri che rappresenta i pixel nel formato RGB
  */
  void fix_pixel_array();
  /**
    \brief Controlla la correttezza del nome del file rispetto al tipo di file BITMAP
    @return true se il nome termina con 'bmp' o 'BMP', false altrimenti
  */
  bool check_bmp_file(const std::string& file_name);

};

/**
  \brief funzione di supporto per effettuare lo scambio tra due elementi
*/
void _swap(unsigned char& a, unsigned char& b);

#endif // BITMAPFILE_H
