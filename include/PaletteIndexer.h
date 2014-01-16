#ifndef PALETTEINDEXER_H
#define PALETTEINDEXER_H

#include <vector>
#include <stdexcept>

/*!
  \brief Struttura rappresentate una tripletta RGB
*/
struct RgbStruct {
public:
  unsigned char r;
  unsigned char g;
  unsigned char b;

  RgbStruct(unsigned char newR, unsigned char newG, unsigned char newB){
    r = newR;
    g = newG;
    b = newB;
  }

  bool operator==(const RgbStruct& rgb) const {
    return (this->r == rgb.r && this->g == rgb.g && this->b == rgb.b);
  }

};

/*!
  \brief Classe che permette di gestire agevolmente la palette di una immagine
  che presuppone la presenza di una palette di colori per la sua corretta apertura.
*/
class PaletteIndexer {
public:
  /*!
    \brief Inizializza questo oggetto adoperando i colori specificati in input.

    @param palette_char colori che la palette dovrà adoperare
    @param num_colors numero di colori da dover gestire

  */
  PaletteIndexer(unsigned char **palette_char, int num_colors);

  PaletteIndexer(const PaletteIndexer& palette);

  PaletteIndexer& operator=(const PaletteIndexer& palette);

  /*!
    \brief Restituisce la posizione nell'indice della tripletta specificata

    @param rgb tripletta RGB della quale si intende sapere la posizione nell'indice
    @return valore positivo che rappresenta la posizione nella palette del colore specificato

    @throw std::logic_error nel caso in cui il colore non fosse presente nella palette
  */
  int get_index(const RgbStruct& rgb) const;
private:
  std::vector<RgbStruct> palette_index; ///< palette di colori attualmente gestita
};


#endif // PALETTEINDEXER_H
