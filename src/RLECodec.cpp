#include "RLECodec.h"
#include "UtilityFunction.h"
#include <stdexcept>

std::vector<unsigned char> RLECodec::decode_array(const std::vector<unsigned char> &vet, BmpCompression compr_mode, int width, int height) {
  // Applico differenti modalità di decompressione in base alla modalità di compressione
  // La tipologia di compressione è fortemente legata alla tipologia di profondità di colore dell'immagine
  // RLE_4 per immagini con profondità di colore a 4 bit
  // RLE_8 per immagini con profondità di colore a 8 bit
  std::vector<unsigned char> ret;

  switch(compr_mode) {
    case BI_RGB:
      return vet;
    case BI_RLE4:
      ret = decode_rle_4(vet, width, height);
      break;
    case BI_RLE8:
      ret = decode_rle_8(vet, width, height);
      break;
  }

  return ret;

}

std::vector<unsigned char> RLECodec::encode_array(const std::vector<unsigned char> &vet, BmpCompression compr_mode, int width, int height) {

  std::vector<unsigned char> ret;

  switch(compr_mode) {
    case BI_RGB:
      return ret;
    case BI_RLE4:
      ret = encode_rle_4(vet, width, height);
      break;
    case BI_RLE8:
      ret = encode_rle_8(vet, width, height);
      break;

  }

  return ret;

}



std::vector<unsigned char> RLECodec::decode_rle_8(const std::vector<unsigned char>& vet, unsigned int width, unsigned int height) {
  std::vector<unsigned char> ret;
  std::vector<unsigned char>::const_iterator it = vet.begin(), ed = vet.end(), ahead = it+1;
  int cont  = 0;
  bool end_bitmap = false;
  unsigned int horiz_move, vert_move;

  while(it != (ed-1) && !end_bitmap) {
    // acquisisco il numero di ripetizioni di un dato pixel
    if(*it != 0) {
      cont = *it;
      for(int i = 0; i < cont; i++)
        ret.push_back(*ahead); // salva cont-volte il carattere successivo
      it += 2;
      ahead = it + 1;
    } else if(*it == 0) {
      // devo riconoscere il tipo di modalità
      switch(*ahead) {
        // Fine della linea
        case 0: it += 2; ahead = it + 1;
          break;
          // Fine del bitmap
        case 1:
          end_bitmap = true;
          break;
          // Delta
        case 2:
          horiz_move = *(it += 2), vert_move = *(it += 1);
          // prevengo eventuali casi d'errore nello scostamento
          if(horiz_move > width || vert_move > height)
            throw std::logic_error("Errore nella decompressione dell'immagine: scostamento non valido");
          it += (horiz_move*width + vert_move);
          break;
          // absolute mode
        default:
          int num_pixel = *ahead, padding = correct_number_byte(num_pixel) - num_pixel;
          ++it; // mi sposto su ahead
          for(int i = 0; i < num_pixel; i++) {
            ret.push_back(*(++it));
          }

          // salta i byte di padding
          for(int i = 0; i < padding; i++)
            ++it;

          ++it; // cursore sul nuovo byte da esaminare
          ahead = it + 1;

      }

    }
  }

  return ret;

}

std::vector<unsigned char> RLECodec::decode_rle_4(const std::vector<unsigned char>& vet, unsigned int width, unsigned int height) {
  std::vector<unsigned char> ret;
  std::vector<unsigned char>::const_iterator it = vet.begin(), ed = vet.end(), ahead = it+1;
  int cont  = 0;
  bool end_bitmap = false;
  unsigned int horiz_move, vert_move;
  unsigned char curr_char;

  while(it != (ed-1) && !end_bitmap) {
    // acquisisco il numero di ripetizioni di un dato pixel
    if(*it != 0) {
      cont = *it;
      // salvo, in maniera alternata, l' indici del colore contenuto nei quattro bit
      // più significativi e l' indice contenuto nella parte meno significativa del secondo byte
      // della coppia. Tale operazione viene svolta per un numero di volte pari al valore contenuto
      // nel primo byte della coppia.
      // I valori che vengono inseriti devono comunque rispettare le modalità di memorizzazione
      // delle immagini a 4-bit, pertanto devo disporre gli indici nelle rispettive nibble di ogni byte
      for(int i = 1; i <= cont; i++) {
        if(i % 2 != 0) { // nibble più significativa
          curr_char = *ahead >> 4;
          curr_char <<= 4;
        } else { // nibble meno significativa
          curr_char += (*ahead & 0x0f);
          ret.push_back(curr_char);
          curr_char = 0;

        }
      }

      it += 2;
      ahead = it + 1;
    } else if(*it == 0) {
      // devo riconoscere il tipo di modalità
      switch(*ahead) {
        // Fine della linea
        case 0: it += 2; ahead = it + 1;
          break;
          // Fine del bitmap
        case 1:
          end_bitmap = true;
          break;
          // Delta, individuo lo scostamento da effettuare per posizionarmi sul pixel corretto
        case 2:
          horiz_move = *(it += 2), vert_move = *(it += 1);
          if(horiz_move > width || vert_move > height)
            throw std::logic_error("Errore nella decompressione dell'immagine: scostamento non valido");
          it += (horiz_move * width + vert_move);
          break;
          // absolute mode
        default:
          int num_pixel = (*ahead/2), // ogni byte contiene 2 colori
              padding = correct_number_byte(num_pixel) - num_pixel;
          ++it; // mi sposto su ahead
          for(int i = 0; i < num_pixel; i++) {
            ret.push_back(*(++it));
          }

          // salta i byte di padding
          for(int i = 0; i < padding; i++)
            ++it;

          ++it; // cursore sul nuovo byte da esaminare
          ahead = it + 1;

      }

    }
  }



  return ret;

}


std::vector<unsigned char> RLECodec::encode_rle_8(const std::vector<unsigned char>& vet, int width, int height) {
  std::vector<unsigned char> ret;
  std::vector<unsigned char>::const_iterator it,
      ed = vet.end(),
      not_rep = ed,
      ahead;
  int num_rep = 1;

  for(int i = 0; i < height; i++) {
    it = vet.begin() + (i*width); // mi posiziono sul primo elemento della riga
    ahead = it + 1;
    for(int j = 0; j <= width && it != ed; j++) {
      // Se ho rilevato due elementi in sequenza uguali incremento
      // il numero di ripetizioni
      if(*it == *ahead) {
        if(not_rep != ed) {
          int num_elem = std::distance(not_rep, it),
              padding = correct_number_byte(num_elem) - num_elem;
          // absolute mode
          ret.push_back(0); // primo byte zero
          ret.push_back(num_elem); // numero di ripetizioni
          ret.insert(ret.end(),not_rep, it);
          // introduco eventuali byte di padding necessari
          for(int i = 0; i < padding; i++)
            ret.push_back(0);

          not_rep = ed;
        }

        // ho incontrato una nuova ripetizione
        if(j != width)
          num_rep++;

      } else { // altrimenti provvedo ad aggiungere num_rep ripetizioni del carattere
        if(num_rep != 1) {
          ret.push_back(num_rep);
          ret.push_back(*it);
          not_rep = ahead;
        }
        num_rep = 1;

      }
      // ho esaminato un pixel
      ++it;
      ahead = it + 1;
    }

    // Se non ho finito i pixel dell'immagine allora inserisco la fine
    // della riga
    if(it != ed) {
      ret.push_back(0);
      ret.push_back(0);
    } else {
      // Ho finito l'immagine, provvedo ad inserire gli eventuali byte rimanenti
      // che non si ripetono
      if(not_rep != ed) {
        int num_elem = std::distance(not_rep, it),
            padding = correct_number_byte(num_elem) - num_elem;
        // absolute mode
        ret.push_back(0); // primo byte zero
        ret.push_back(num_elem); // numero di ripetizioni
        ret.insert(ret.end(),not_rep, it);
        // introduco eventuali byte di padding necessari
        for(int i = 0; i < padding; i++)
          ret.push_back(0);

      }
    }
  }

  ret.push_back(0);
  ret.push_back(1);

  return ret;
}

// NON ANCORA IMPLEMENTATA
std::vector<unsigned char> RLECodec::encode_rle_4(const std::vector<unsigned char>& vet, int width, int height) {
  std::vector<unsigned char> ret, abs_mode;
  std::vector<unsigned char>::const_iterator curr_elem, future;
  int cont_rep = 1, curr_char, cont_future;
  int num_byte = (width%2 == 0) ? width/2 : width/2 + 1, num_abs, padding;

  // per ogni riga dell'immagine
  for(int i = 0; i < height; i++) {

    // il primo elemento è il primo valore della riga
    curr_elem = vet.begin() + (i*num_byte); // puntatore al primo elemento della riga
    curr_char = *curr_elem; // primo elemento della riga (parto da questo nel controllo)
    ++curr_elem; // mi sposto sul secondo elemento
    cont_rep = 1;

    for(int j = 1; j < num_byte; j++) {
      if(curr_char == *curr_elem) {
        // se ho degli elementi da inserire provenienti dall'absolute mode
        if(!abs_mode.empty()) {
          cont_future = cont_rep+1;

          for(int k = j+1; k < num_byte && *(curr_elem+k) == curr_char; k++)
            cont_future++; // conta occorrenze possibili

          if((cont_future*2) >= 8) {
            num_abs = abs_mode.size();
            ret.push_back(0);
            ret.push_back(num_abs);
            ret.insert(ret.end(), abs_mode.begin(), abs_mode.end());
            padding = correct_number_byte(num_abs) - num_abs;
            for(int k = 0; k < padding; k++)
              ret.push_back(0);
            abs_mode.clear();
            cont_rep = 1;
          }
        }
        cont_rep++;

      } else { // trovato elemento diverso
        // salvo solo valori ripetuti più di una volta (encoded mode)
        if((cont_rep*2) >= 8) {
          ret.push_back(cont_rep*2); // ogni byte contiene 2 nibble
          ret.push_back(curr_char);
          // carattere da controllare cambia
          curr_char = *curr_elem;
          cont_rep = 1;
        } else { // sono in absolute mode
          // Devo far si che vengano inserite tutte le ripetizioni
          // che non fanno parte della modalità encoded
          for(int k = 0; k < cont_rep; k++)
            abs_mode.push_back(curr_char);
          cont_rep = 1;
          curr_char = *curr_elem;
        }
      }

      // vado al carattere successivo
      ++curr_elem;
    }

    // devo completare con absolute mode
    if(!abs_mode.empty()) {
      for(int k = 0; k < cont_rep; k++)
        abs_mode.push_back(curr_char); // inserisco l'ultimo rimanente
      num_abs = abs_mode.size();
      ret.push_back(0);
      ret.push_back(num_abs*2);
      ret.insert(ret.end(), abs_mode.begin(), abs_mode.end());
      padding = correct_number_byte(num_abs) - num_abs;
      for(int k = 0; k < padding; k++)
        ret.push_back(0);
      abs_mode.clear();
    } else { // completo con encoded
      ret.push_back(cont_rep*2);
      ret.push_back(curr_char);
      curr_char = 0;
      cont_rep = 1;
    }
    //fine della riga corrente
    ret.push_back(0);
    ret.push_back(0);
  }

  // fine del bitmap
  ret.push_back(0);
  ret.push_back(1);

  return ret;
}

std::vector<unsigned char> generate_vet_nibble(std::vector<unsigned char>::const_iterator start, std::vector<unsigned char>::const_iterator end_it ) {
  std::vector<unsigned char> vet_nibble;

  for(std::vector<unsigned char>::const_iterator it = start, ed = end_it;
      it != ed;
      ++it) {
    vet_nibble.push_back(*it >> 4);
    vet_nibble.push_back(*it & 0x0f);
  }

  return vet_nibble;
}

