#include "RLECodec.h"

std::vector<unsigned char> RLECodec::decode_array(const std::vector<unsigned char> &vet, BmpCompression compr_mode) {
  // Applico differenti modalità di decompressione in base alla modalità di compressione
  // La tipologia di compressione è fortemente legata alla tipologia di profondità di colore dell'immagine
  // RLE_4 per immagini con profondità di colore a 4 bit
  // RLE_8 per immagini con profondità di colore a 8 bit
  std::vector<unsigned char> ret;

  switch(compr_mode) {
    case BI_RGB:
      return vet;
    case BI_RLE4:
      break;
    case BI_RLE8:
      break;
  }

  return ret;

}

std::vector<unsigned char> RLECodec::encode_array(const std::vector<unsigned char> &vet, BmpCompression compr_mode) {

  std::vector<unsigned char> ret;
  int curr_run_length = 1;
  unsigned char curr_sym;


  switch(compr_mode) {
    case BI_RGB:
      return ret;
    case BI_RLE4:
      break;
    case BI_RLE8:
      if(vet.empty())
        return ret;
      else if (vet.size() == 2)
        return vet;

      std::vector<unsigned char>::const_iterator it = vet.begin(),
          ed = vet.end(), ahead = it+1;

      // Analizzo l'intera sequenza di byte (ogni byte rappresenta un pixel)
      // salvo il carattere corrente
      curr_sym = *it;

      while(it != ed) {
        if(*it == *ahead) {
          // nuova occorrenza trovata
          curr_run_length++;
          // procedo nell'analisi della sequenza di byte
          it = ahead;
          ahead = it+1;
        } else {
          // sequenza interrotta: salvo i dati nella forma (#OCC, DATO)
          ret.push_back(curr_run_length);
          ret.push_back(curr_sym);
          // azzero la lunghezza della sottosequenza
          curr_run_length = 1;
          // salvo il nuovo carattere della possibile sottosequenza
          it = ahead;
          curr_sym = *it;
          // look-ahead
          ahead = it+1;

        }


      }

      break;

  }

  return ret;

}


