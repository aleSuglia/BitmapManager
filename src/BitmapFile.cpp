#include "BitmapFile.h"
#include "UtilityFunction.h"
#include <cmath>
#include <cstring>
#include <sstream>

BitmapFile::BitmapFile(const std::string& file_name) :
  reversed_pixel_data(std::vector<unsigned char>()),
  original_pixel_data(std::vector<unsigned char>()){
  dib_header.palette = NULL;
  read_bmp_file(file_name);
}

BitmapFile::BitmapFile(BitmapTO& to) :
  reversed_pixel_data(to.getPixelData()),
  original_pixel_data(std::vector<unsigned char>()) {

  fill_bmp_header(to.getWidth(), to.getHeight(), to.getBitsPerPixel(), to.getBitmapSize(),
                  (to.getBitsPerPixel() <= 8) ? (static_cast<BitmapTOwP&>(to)).getPalette() : 0, to.getComprMode());
  retrieve_original_pixel_data();

}

BitmapFile::BitmapFile(const BitmapFile& bmp) {
  *this = bmp;
}

BitmapFile& BitmapFile::operator=(const BitmapFile& bmp) {
  if(this != &bmp) {
    this->original_pixel_data = bmp.original_pixel_data;
    this->reversed_pixel_data = bmp.reversed_pixel_data;
    this->file_header = bmp.file_header;
    this->dib_header = bmp.dib_header;
  }

  return *this;
}

BitmapFile::~BitmapFile() {

  // dealloco la memoria 'eventualmente' allocata per la palette di colori
  // Questa verrà allocata unicamente per immagini con bit_depth <= 8
  if(dib_header.palette != 0) {
    for(unsigned int i = 0; i < dib_header.num_colors; i++)
      delete [] dib_header.palette[i];

    delete [] dib_header.palette;
  }
}
/**
  Costruisce l'immagine corrente a partire dal contenuto
  del file il cui nome è specificato in input al metodo

  */
void BitmapFile::read_bmp_file(const std::string& file_name) {
  // Controllo che il file abbia estensione BMP
  // (controllo superfluo visto che nell'interfaccia grafica vengono filtrati i file per estensione)

  if(!check_bmp_file(file_name)) {
    throw std::logic_error("Il file selezionato non è nel formato BMP");
  }

  std::ifstream file_stream(file_name.c_str(), std::ios::in | std::ios::binary);

  if(file_stream.fail()) {
    throw std::logic_error("Percorso del file o directory errata");
  }

  // Legge tutti i componenti del File header
  file_stream.read((char*)&file_header.file_type, sizeof file_header.file_type);
  file_header.file_type[2] = '\0';
  if(strcmp(file_header.file_type, "BM") != 0)
    throw std::logic_error("Il tipo di file non risulta essere corretto");
  file_stream.read((char*)&file_header.file_size, sizeof file_header.file_size);
  file_stream.read((char*)&file_header.reserved1, sizeof file_header.reserved1);
  file_stream.read((char*)&file_header.reserved2, sizeof file_header.reserved2);
  file_stream.read((char*)&file_header.data_offset, sizeof file_header.data_offset);

  // Legge tutti i componenti del Dib header
  file_stream.read((char*)&dib_header.header_size, sizeof dib_header.header_size);
  file_stream.read((char*)&dib_header.width, sizeof dib_header.width);
  file_stream.read((char*)&dib_header.height, sizeof dib_header.height);
  file_stream.read((char*)&dib_header.planes, sizeof dib_header.planes);
  file_stream.read((char*)&dib_header.bits_per_pixel, sizeof dib_header.bits_per_pixel);
  file_stream.read((char*)&dib_header.compression, sizeof dib_header.compression);
  file_stream.read((char*)&dib_header.bitmap_size, sizeof dib_header.bitmap_size);
  file_stream.read((char*)&dib_header.horizontal_res, sizeof dib_header.horizontal_res);
  file_stream.read((char*)&dib_header.vertical_res, sizeof dib_header.vertical_res);
  file_stream.read((char*)&dib_header.num_colors, sizeof dib_header.num_colors);
  file_stream.read((char*)&dib_header.important_colors, sizeof dib_header.important_colors);


  if(!is_dib_header_valid()) {
    throw std::logic_error("Sono state trovate delle anomalie nel formato del file bitmap");
  }

  // Se l'immagine presenta una profondità minore di 16 allora è necessario far riferimento
  // alla tabella di colori specificata
  // dib_header.num_colors indica i colori adoperati e la dimensione della rispettiva tabella di colori

  if(dib_header.bits_per_pixel <= 8) {
    // numero di colori della palette Ogni elemento della palette è nel formato (B,G,R, RES)
    int num_colors = (file_header.data_offset-54)/4;
    // se il numero di colori della palette è maggiore di 256 la palette risulta essere corrotta
    if(num_colors > 256)
      throw std::logic_error("La palette dell'immagine non risulta essere valida");

    dib_header.palette = new unsigned char*[num_colors];
    for(int i = 0; i < num_colors; i++) {
      dib_header.palette[i] = new unsigned char[4];
      file_stream.read((char*) dib_header.palette[i], 4);
    }

  }


  if(!original_pixel_data.empty()) {
    original_pixel_data.clear();
    reversed_pixel_data.clear();
  }

  // Il valore relativo alla dimensione dell'array di pixel potrebbe essere pari a zero
  // In questo caso è necessario effettuare manualmente il calcolo della dimensione per
  // poter operare correttamente in fase di lettura dell'immagine
  int real_bitmap_size = (dib_header.bitmap_size == 0) ? dib_header.width *
                                                         (dib_header.bits_per_pixel / 8) * dib_header.height :
                                                         dib_header.bitmap_size;

  original_pixel_data.assign(real_bitmap_size, 0);
  // Legge la parte relativa al contenuto dell'immagine vero e proprio
  file_stream.read((char*)&original_pixel_data[0], real_bitmap_size);
  /*
      Se dib_header.height > 0 i pixel risultano essere disposti in verso
      inverso, ovvero ogni pixel viene memorizzato  dal basso(a sinistra) verso l'alto.
      (0,0)B, (0,1)G; (1,0)R, (1,1)W; -> R(1,0)-W(1,1)-B(0,1)-(0,0)G
  */
  // Controllo che l'immagine non sia stata compressa con un algoritmo di compressione
  // Se fosse stata compressa, provvedo a decomprimerla recuperando i pixel
  // Questi sarebbero a questo punto nella forma definita dalla profondità di colore corrente
  if(dib_header.compression != 0)
    reversed_pixel_data = RLECodec::decode_array(original_pixel_data, (BmpCompression) dib_header.compression, dib_header.width, dib_header.height);

  // procedi alla normale lettura dell'array di pixel
  fix_pixel_array();

  file_stream.close();

  print_bitmap_information();


}

void BitmapFile::fix_pixel_array() {
  /*
      Effettuo una rotazione delle triplette in modo tale da ottenere
      l'array di pixel con le triplette disposte in modo corretto.
      Inizialmente, infatti, le triplette sono disposte nella forma BGR
      di conseguenza ho la necessità di invertirle in modo da ottenere
      la classica tripletta RGB.
  */
  int tot_size;
  int height = dib_header.height, width = dib_header.width;

  // Se l'immagine corrente fosse compressa, avrei una differente dimensione
  // della sezione dati una volta effettuata la decompressione
  if(dib_header.compression != 0)
    tot_size = reversed_pixel_data.size();
  else
    tot_size = (dib_header.bitmap_size == 0 ) ? original_pixel_data.size() : dib_header.bitmap_size;


  // Procedo nell'interpretazione dei byte della sezione dati dell'immagine secondo quella che
  // è la profondità di colore e la eventuale modalità di compressione
  switch(dib_header.bits_per_pixel) {
    case 24: {

        // se la larghezza dell'immagine non è multiplo di quattro, rimuovo il padding
        if(width%4 != 0) {
          int padding = correct_number_byte(width*3) - width*3, cont_pixel = 0;
          reversed_pixel_data.clear();

          int pos = 0, tot_byte = original_pixel_data.size();

          while(pos < tot_byte) {
            reversed_pixel_data.push_back(original_pixel_data[pos]);
            cont_pixel++;
            if(cont_pixel / 3 == width) {
              cont_pixel = 0;
              for(int i = 0; i < padding; i++)
                ++pos;

            }

            ++pos;

          }

          // aggiorno la dimensione del vettore
          tot_size = reversed_pixel_data.size();


        } else
          reversed_pixel_data = original_pixel_data;

        for(int i = 0; i <= tot_size-2; i+=3)
          _swap(reversed_pixel_data[i], reversed_pixel_data[i+2]);


        /*
       Accorpo le varie triplette in modo tale da poterle gestire in maniera
       più semplice
      */
        std::vector<RgbStruct> rgb_struct;

        for(int i = 0; i <= tot_size-2; i+=3)
          rgb_struct.push_back(RgbStruct(reversed_pixel_data[i], reversed_pixel_data[i+1], reversed_pixel_data[i+2]));

        /* Ricreo l'array di pixel_data effettuando un'operazione di rotazione sul vettore
       Gli elementi devono necessariamente essere letti dal basso verso l'alto e da sinistra
       verso destra */

        reversed_pixel_data.clear();
        for(int i = height-1; i >= 0; i--) {
          for(int j = 0; j < width; j++) {
            reversed_pixel_data.push_back(rgb_struct[i*width+j].r);
            reversed_pixel_data.push_back(rgb_struct[i*width+j].g);
            reversed_pixel_data.push_back(rgb_struct[i*width+j].b);
          }
        }
      }
      break;
    case 1: {
        // scansiono l'array di pixel e ne genero un secondo
        // che contenga le triplette RGB prelevandole dalla palette dell'immagine
        std::vector<RgbStruct> rgb_vect;
        int num_relevant_byte = 0, useless_byte = 0;
        // Se il numero di pixel memorizzati su una riga
        // supera 32 (sfrutto più di 32bit per memorizzare una riga)
        if(width > 32) {
          if((width%8) == 0) {
            num_relevant_byte = (width/8);
          } else {
            num_relevant_byte = (width/8)+1;
          }
          useless_byte = correct_number_byte(num_relevant_byte) - num_relevant_byte;
        } else { // per memorizzare una riga ho bisogno di meno di 32bit
          useless_byte = (32-width)/8;
          num_relevant_byte = 4-useless_byte;
        }
        int cont_pixel = 0;

        std::vector<unsigned char>::const_iterator it = original_pixel_data.begin(),
            ed = original_pixel_data.end();

        while(it != ed) {
          std::vector<bool> bitset = char_to_bitset(*it);
          for(std::vector<bool>::const_iterator it_bit = bitset.begin(),
              ed_bit = bitset.end();
              it_bit != ed_bit && cont_pixel < width;
              ++it_bit) {
            rgb_vect.push_back(RgbStruct(dib_header.palette[*it_bit][2], dib_header.palette[*it_bit][1], dib_header.palette[*it_bit][0]));
            cont_pixel++;

          }

          // ignora bit di padding
          if(cont_pixel == width) {
            it += useless_byte; // procedo in avanti di "useless_byte"-byte
            cont_pixel = 0;
          }

          ++it; // avanza nel vettore di caratteri
        }


        // L'immagine è stata memorizzata dall'alto verso il basso
        // Non ho la necessità di capovolgere i pixel
        if(dib_header.height < 0) {
          dib_header.height = dib_header.height*-1;
          for(std::vector<RgbStruct>::const_iterator it = rgb_vect.begin(),
              ed = rgb_vect.end();
              it != ed;
              ++it) {
            reversed_pixel_data.push_back(it->r);
            reversed_pixel_data.push_back(it->g);
            reversed_pixel_data.push_back(it->b);

          }

        } else {
          for(int i = height-1; i >= 0; i--) {
            for(int j = 0; j < width; j++) {
              reversed_pixel_data.push_back(rgb_vect[i*width+j].r);
              reversed_pixel_data.push_back(rgb_vect[i*width+j].g);
              reversed_pixel_data.push_back(rgb_vect[i*width+j].b);
            }
          }
        }

      } break;
    case 4: {
        // scansiono l'array di pixel e ne genero un secondo
        // che contenga le triplette RGB prelevandole dalla palette dell'immagine
        std::vector<RgbStruct> rgb_vect;
        int num_relevant_byte, useless_byte, num_nibble, color_index;
        num_relevant_byte = (width%2==0) ? width/2 : (width/2)+1;

        if(width > 8) {
          useless_byte = correct_number_byte(num_relevant_byte) - num_relevant_byte;
        } else {
          // i pixel di una riga dell'immagine
          // possono essere memorizzati in 4 byte (32bit)
          useless_byte = 4 - num_relevant_byte;
        }

        /*
          Lavoro sempre su reversed_pixel_data in modo tale da poter gestire
          adeguatamente il caso in cui si tratti di un immagine compressa
          con RLE_4.
          In questo caso, l'array da dover esplicitare sarà già preparato in
          reversed_pixel_data.
        */
        if(dib_header.compression == 0)
          reversed_pixel_data = original_pixel_data;


        std::vector<unsigned char>::const_iterator it = reversed_pixel_data.begin(),
            ed = reversed_pixel_data.end();
        num_nibble = 1;

        while(it != ed) {
          // il numero di pixel esaminati è minore della grandezza di una riga dell'immagine
          if(num_nibble <= width) {
            if((num_nibble % 2)!= 0)
              color_index = *it >> 4;
            else
              color_index = *it & 0x0f;

            rgb_vect.push_back(RgbStruct(dib_header.palette[color_index][2], dib_header.palette[color_index][1], dib_header.palette[color_index][0]));


            if((num_nibble%2) == 0 && num_nibble != width)
              ++it;
            num_nibble++;

          } else {
            for(int i = 0; i < useless_byte; i++)
              ++it;

            // ricomincio a scandire la linea
            num_nibble = 1;
            ++it; // mi posiziono sul nuovo byte
          }


        }

        // ripulisco il vettore per poter accogliere le triplette RGB corrette
        reversed_pixel_data.clear();

        // L'immagine è stata memorizzata dall'alto verso il basso
        // Non ho la necessità di capovolgere i pixel
        if(dib_header.height < 0) {
          dib_header.height = dib_header.height*-1;
          for(std::vector<RgbStruct>::const_iterator it = rgb_vect.begin(),
              ed = rgb_vect.end();
              it != ed;
              ++it) {
            reversed_pixel_data.push_back(it->r);
            reversed_pixel_data.push_back(it->g);
            reversed_pixel_data.push_back(it->b);

          }

        } else {
          for(int i = height-1; i >= 0; i--) {
            for(int j = 0; j < width; j++) {
              reversed_pixel_data.push_back(rgb_vect[i*width+j].r);
              reversed_pixel_data.push_back(rgb_vect[i*width+j].g);
              reversed_pixel_data.push_back(rgb_vect[i*width+j].b);
            }
          }
        }


      } break;
    case 8:
      {
        std::vector<RgbStruct> vect_rgb;
        int useless_byte = (width % 4 != 0) ? correct_number_byte(width) - width : 0;
        int cont_byte = 0;

        /*
          Lavoro sempre su reversed_pixel_data in modo tale da poter gestire
          adeguatamente il caso in cui si tratti di un immagine compressa
          con RLE_8.
          In questo caso, l'array da dover esplicitare sarà già preparato in
          reversed_pixel_data.
        */
        if(dib_header.compression == 0)
          reversed_pixel_data = original_pixel_data;

        std::vector<unsigned char>::const_iterator it = reversed_pixel_data.begin(),
            ed = reversed_pixel_data.end();


        while(it != ed) {

          // inserisco la nuova tripletta
          vect_rgb.push_back(RgbStruct(dib_header.palette[*it][2], dib_header.palette[*it][1], dib_header.palette[*it][0]));

          cont_byte++;
          // Ho esaurito i byte rilevanti nell'immagine, scarto i fill byte
          if(cont_byte == width) {
            for(int i = 0; i != useless_byte; i++)
              ++it;
            cont_byte = 0;

          }

          // passo al byte successivo
          // effettuo tale operazione anche nel momento in cui ho
          // scartato dei byte in quanto ho la necessità di posizionarmi sulla
          // nuova sequenza di byte rilevanti.
          ++it;
        }

        // ripulisco l'array e lo preparo per poter accogliere le triplette RGB
        reversed_pixel_data.clear();

        // L'immagine è stata memorizzata dall'alto verso il basso
        // Non ho la necessità di capovolgere i pixel
        if(dib_header.height < 0) {
          dib_header.height = dib_header.height*-1;
          for(std::vector<RgbStruct>::const_iterator it = vect_rgb.begin(),
              ed = vect_rgb.end();
              it != ed;
              ++it) {
            reversed_pixel_data.push_back(it->r);
            reversed_pixel_data.push_back(it->g);
            reversed_pixel_data.push_back(it->b);

          }
        } else {

          for(int i = height-1; i >= 0; i--) {
            for(int j = 0; j < width; ++j) {
              reversed_pixel_data.push_back(vect_rgb[i*width+j].r);
              reversed_pixel_data.push_back(vect_rgb[i*width+j].g);
              reversed_pixel_data.push_back(vect_rgb[i*width+j].b);
            }
          }

        }

      }
      break;

  }

}

void _swap(unsigned char& a, unsigned char& b) {
  unsigned char temp = a;
  a = b;
  b = temp;
}



unsigned char* BitmapFile::get_data_array() {
  return &reversed_pixel_data[0];
}

int BitmapFile::bitmap_size() {
  return dib_header.bitmap_size;
}

int BitmapFile::height_size() {
  return dib_header.height;
}

int BitmapFile::width_size() {
  return dib_header.width;
}


void BitmapFile::print_bitmap_information() {
  std::cout << "Filesize: " << file_header.file_size << std::endl;
  std::cout << "Filetype: " << file_header.file_type << std::endl;
  std::cout << "Data offset: " << file_header.data_offset << std::endl;
  std::cout << "Width: " << dib_header.width << std::endl;
  std::cout << "Height: " << dib_header.height << std::endl;
  std::cout << "Bitmap size: " << dib_header.bitmap_size << std::endl;
  std::cout << "Bits per pixel: " << dib_header.bits_per_pixel << std::endl;
  std::cout << "Compression: " << dib_header.compression << std::endl;
  std::cout << "Number of colors: " << dib_header.num_colors << std::endl;

  print_pixel_data(original_pixel_data);
  //print_pixel_data(reversed_pixel_data);
}

void BitmapFile::print_pixel_data(const std::vector<unsigned char>& vet) {
  std::fstream file("out.txt", std::ios::out);
  for(std::vector<unsigned char>::const_iterator it = vet.begin(),
      ed = vet.end();
      it != ed;
      ++it)
    file << (int)*it << std::endl;

  file.close();

}

/**
  Copia l'immagine corrente in un file avente come nome quello specificato
  in input

  */
void BitmapFile::copy_bmp(const std::string& file_name) {

  if(!original_pixel_data.empty()) {
    if(!check_bmp_file(file_name)) {
      throw std::logic_error("Impossibile salvare un file che non ha estensione bmp!");
    }
    std::ofstream file_stream(file_name.c_str(), std::ios::out | std::ios::binary);
    if(file_stream.fail()) {
      throw std::logic_error("Impossibile scrivere il file");
    }

    // Scrive tutte le componenti fondamentali del FileHeader del file bitmap

    file_stream.write((char*)&file_header.file_type, sizeof file_header.file_type);
    file_stream.write((char*)&file_header.file_size, sizeof file_header.file_size);
    file_stream.write((char*)&file_header.reserved1, sizeof file_header.reserved1);
    file_stream.write((char*)&file_header.reserved2, sizeof file_header.reserved2);
    file_stream.write((char*)&file_header.data_offset, sizeof file_header.data_offset);

    // Scrive tutte le componenti fondamentali del DibHeader del file bitmap
    file_stream.write((char*)&dib_header.header_size, sizeof dib_header.header_size);
    file_stream.write((char*)&dib_header.width, sizeof dib_header.width);
    file_stream.write((char*)&dib_header.height, sizeof dib_header.height);
    file_stream.write((char*)&dib_header.planes, sizeof dib_header.planes);
    file_stream.write((char*)&dib_header.bits_per_pixel, sizeof dib_header.bits_per_pixel);
    file_stream.write((char*)&dib_header.compression, sizeof dib_header.compression);
    file_stream.write((char*)&dib_header.bitmap_size, sizeof dib_header.bitmap_size);
    file_stream.write((char*)&dib_header.horizontal_res, sizeof dib_header.horizontal_res);
    file_stream.write((char*)&dib_header.vertical_res, sizeof dib_header.vertical_res);
    file_stream.write((char*)&dib_header.num_colors, sizeof dib_header.num_colors);
    file_stream.write((char*)&dib_header.important_colors, sizeof dib_header.important_colors);

    // Devo procedere nel salvataggio della palette di colori
    if(dib_header.bits_per_pixel <= 8) {
      int num_colors = (file_header.data_offset - 54)/4;

      //std::cout << "(Save) palette: " << std::endl;
      for(int i = 0; i < num_colors; i++) {
        file_stream.write((char*)dib_header.palette[i], 4);
      }
    }

    // Scrive la parte dei dati dell'immagine
    file_stream.write((char*)&original_pixel_data[0], dib_header.bitmap_size);
    file_stream.close();
  }
}

/**
  Restituisce un transfer object con i dati necessari alle view
  per effettuare le operazioni sull'immagine

  */
BitmapTO* BitmapFile::getTO() {
  if(this->original_pixel_data.empty()) {
    return 0;
  }

  /*
    Controllo che tipologia di immagine è rappresentata dall'istanza corrente.
    Se presenta una palette allora restituisco un'istanza ad un oggetto di tipo
    BitmapTOwP altrimenti restituisco un normale BitmapTO
  */

  return  (dib_header.bits_per_pixel <= 8) ?
        new BitmapTOwP(reversed_pixel_data, this->dib_header.width, this->dib_header.height, this->dib_header.bitmap_size,
                       this->dib_header.bits_per_pixel, this->dib_header.palette, getInfoBitmap(), this->dib_header.compression) :
        new BitmapTO(reversed_pixel_data, this->dib_header.width, this->dib_header.height, this->dib_header.bitmap_size, this->dib_header.bits_per_pixel, getInfoBitmap(), this->dib_header.compression);
}

/**
  Crea gli header dell'immagine Bitmap che contengono
  le informazioni fondamentali riguardanti l'immagine
*/
void BitmapFile::fill_bmp_header(int width, int height, int bits_per_pixel, int bitmap_size, unsigned char **palette, int compr_mode) {
  int num_color_palette = (bits_per_pixel <= 8) ? pow(2,bits_per_pixel) : 0,
      data_offset = (bits_per_pixel <= 8) ? 54 + num_color_palette*4 : 54;

  // Inizializzo gli header
  memset(&file_header, 0, sizeof(file_header));
  memset(&dib_header, 0, sizeof(dib_header));

  // sigla fissa presente nelle immagini bitmap
  strcpy(file_header.file_type, (const char*)"BM");
  // La dimensione del file è sempre pari alla dimensione dell'array di pixel più la dimensione dei due header
  file_header.file_size = bitmap_size+data_offset;
  // Valori numerici riservati
  file_header.reserved1 = 0;
  file_header.reserved2 = 0;
  // fine valori numerici riservati

  // Offset mediante il quale effettuare l'accesso alla sezione dei dati
  file_header.data_offset = data_offset;

  // dimensione dell'header corrente
  // è necessario valutare se è presente la palette o meno
  dib_header.header_size = 40;
  // grandezza della sezione dati
  dib_header.bitmap_size = bitmap_size;
  // profondità del colore
  dib_header.bits_per_pixel = bits_per_pixel;
  // risoluzione orizzontale dell'immagine
  dib_header.horizontal_res = 0;
  // risoluzione verticale dell'immagine
  dib_header.vertical_res = 0;
  // Larghezza dell'immagine
  dib_header.width = width;
  // altezza dell'immagine
  dib_header.height = height;
  // numero di piani sui quali si sviluppa l'immagine (valore fisso 1)
  dib_header.planes = 1;

  // ATTUALMENTE NON ANCORA SUPPORTATO. Scrivo le immagini prive di compressione
  //dib_header.compression = compr_mode;
  // Avvalora il campo dell'header contenente la palette
  if(palette != 0) {
    dib_header.palette = new unsigned char *[num_color_palette];
    for(int i = 0; i < num_color_palette; i++) {
      dib_header.palette[i] = new unsigned char[4];
      std::copy(&palette[i][0], &palette[i][3], &dib_header.palette[i][0]);


    }
  }

}


/**
    Ripristina l'array reversed_pixel_data (che rappresenta i pixel dell'immagine)
    nel formato originale di un immagine di tipo Bitmap
  */
void BitmapFile::retrieve_original_pixel_data() {

  original_pixel_data = reversed_pixel_data;
  int tot_size = original_pixel_data.size();
  int height = dib_header.height, width = dib_header.width;

  /*
    Inizialmente si preferisce lavorare con la rappresentazione dell'immagine
    mediante triplette RGB in modo tale da poter gestire comodamente tutti i casi.
    In seguito per ogni tipo di profondità di colore supportata verranno creati
    gli array di byte specifici che ovviamente dovranno sottostare a quelle che sono
    le specifiche fornite per la memorizzazione dei dati dell'immagine secondo quella
    definita profondità di colore.
  */

  /*
      Accorpo le varie triplette in modo tale da poterle gestire in maniera
      più semplice
    */
  std::vector<RgbStruct> rgb_struct;
  for(int i = 0; i <= tot_size-2; i+=3)
    rgb_struct.push_back(RgbStruct(original_pixel_data[i], original_pixel_data[i+1], original_pixel_data[i+2]));


  /* Ricreo l'array di pixel_data effettuando un'operazione di rotazione sul vettore
   Gli elementi devono necessariamente essere letti dal basso verso l'alto e da sinistra
   verso destra */

  original_pixel_data.clear();

  /*
    Nonostante sia possibile trovare delle immagini BITMAP che risultano essere
    memorizzate dal all'alto verso il basso (come normalmente noi le vedremmo su un monitor)
    lo standard del formato BITMAP impone che queste vengano memorizzate dal basso (a sinistra)
    verso l'alto.
    Pertanto è necessario effettuare un'operazione di rotazione dei pixel per poterli disporre
    correttamente nel vettore.
    */
  for(int i = height-1; i >= 0; i--) {
    for(int j = 0; j < width; j++) {
      original_pixel_data.push_back(rgb_struct[i*width+j].b);
      original_pixel_data.push_back(rgb_struct[i*width+j].g);
      original_pixel_data.push_back(rgb_struct[i*width+j].r);

    }
  }


  /*
   *
    Costruisco un hashmap a partire dalla palette in modo tale
    da velocizzare la procedura di inserimento dei pixel
  */
  if(dib_header.bits_per_pixel <= 8) {

    std::vector<unsigned char> temp_array = original_pixel_data;

    original_pixel_data.clear();
    int num_colors = pow(2,dib_header.bits_per_pixel);
    PaletteIndexer palette_index(dib_header.palette, num_colors);

    switch(dib_header.bits_per_pixel) {
      case 1:
        generate_pixel_array_1(palette_index, temp_array);
        break;
      case 4:
        generate_pixel_array_4(palette_index, temp_array);
        break;
      case 8:
        generate_pixel_array_8(palette_index, temp_array);
        break;
    }

    // Provvede ad effettuare una compressione dei dati nel caso fosse necessario
    // Non supportato completamente //
    /*
    if(dib_header.compression != 0)
      original_pixel_data = RLECodec::encode_array(original_pixel_data, (BmpCompression) dib_header.compression, dib_header.width, dib_header.height);
    */

  } else {
    /*
      Nel caso in cui l'immagine non avesse come larghezza
      un valore multiplo di 4, prima della memorizzazione della stessa
      è necessario allineare ogni singola riga che la compone a 32 bit.
      Ovvero è necessario introdurre un numero di byte pari a '0' chiamati
      byte di padding.
    */
    if(dib_header.width%4 != 0)
      generate_pixel_array_24();
  }
}


void BitmapFile::generate_pixel_array_24() {
  size_t bytesPerPixel = 3,
      row_len = bytesPerPixel*dib_header.width, // numero di elementi in una riga dell'immagine
      padding = correct_number_byte(row_len) - row_len, // numero di byte di padding
      array_size = original_pixel_data.size(),
      tot_dim_aligned = array_size + (dib_header.height*padding),
      aligned_cont = 0;

  std::vector<unsigned char> pixelData = original_pixel_data;

  original_pixel_data.clear();

  original_pixel_data.resize(tot_dim_aligned,0);

  std::vector<unsigned char>::iterator it = pixelData.begin(), orig_iter = original_pixel_data.begin();

  for(size_t cont_pixel = 0; cont_pixel < array_size; cont_pixel += row_len) {
    std::copy(it+cont_pixel, it+(cont_pixel+row_len), orig_iter+aligned_cont);
    aligned_cont += row_len + padding; // vado avanti di padding-byte
  }
}

void BitmapFile::generate_pixel_array_1(const PaletteIndexer& palette_index, const std::vector<unsigned char>& temp_array) {
  int width = dib_header.width,
      num_relevant_byte = 0,
      useless_byte = 0,
      scanline_width = width*3;
  // Se il numero di pixel memorizzati su una riga
  // supera 32 (sfrutto più di 32bit per memorizzare una riga)
  if(width > 32) {
    if((width%8) == 0) {
      num_relevant_byte = (width/8);
    } else {
      num_relevant_byte = (width/8)+1;
    }
    useless_byte = correct_number_byte(num_relevant_byte) - num_relevant_byte;
  } else { // per memorizzare una riga ho bisogno di meno di 32bit
    useless_byte = (32-width)/8;
    num_relevant_byte = 4-useless_byte;
  }

  std::vector<bool> bit_array;

  for(int i = 0, height = height_size(); i < height; i++) {
    // per ogni elemento effettivo della scanline corrente
    for(int j = 0; j <= scanline_width-2; j += 3) {
      // Acquisisco l'indice della palette relativo alla tripletta corrente
      bit_array.push_back(palette_index.get_index(RgbStruct(temp_array[i*scanline_width+(j+2)],
                                                  temp_array[i*scanline_width+(j+1)],temp_array[i*scanline_width+j])));
      // Se ho completato un ottetto
      if(bit_array.size() == 8) {
        original_pixel_data.push_back(bitset_to_char(bit_array));
        bit_array.clear();
      }
    }

    if(bit_array.size() != 0 && bit_array.size() != 8) {
      // Completo l'array di bit per costituire un ottetto
      for(int cont = bit_array.size(); cont < 8; cont++)
        bit_array.push_back(0);
      original_pixel_data.push_back(bitset_to_char(bit_array));
      bit_array.clear();

    }

    // inserisco byte di padding necessari per completare la scanline corrente
    for(int pad = 0; pad < useless_byte; pad++)
      original_pixel_data.push_back(0);
  }

}

void BitmapFile::generate_pixel_array_4(const PaletteIndexer& palette_index, const std::vector<unsigned char>& temp_array) {
  unsigned char curr_pixel;
  int width = dib_header.width,
      height = dib_header.height,
      scanline_width = width*3,
      num_relevant_byte = (width % 2 == 0) ? width/2 : width/2+1,
      useless_byte = 0;

  if(width > 8) {
    useless_byte = correct_number_byte(num_relevant_byte) - num_relevant_byte;
  } else {
    // i pixel di una riga dell'immagine
    // possono essere memorizzati in 4 byte (32bit)
    useless_byte = 4 - num_relevant_byte;
  }

  int num_nibble = 1;

  for(int i = 0; i < height; i++) {
    // per ogni elemento effettivo della scanline corrente
    for(int j = 0; j <= scanline_width-2; j += 3) {
      // nibble più significativo
      if(num_nibble % 2 != 0) {
        curr_pixel = palette_index.get_index(RgbStruct(temp_array[i*scanline_width+(j+2)], temp_array[i*scanline_width+(j+1)],
            temp_array[i*scanline_width+j]));
        curr_pixel <<= 4;
      } else { // nibble meno significativo
        curr_pixel += palette_index.get_index(RgbStruct(temp_array[i*scanline_width+(j+2)], temp_array[i*scanline_width+(j+1)],
            temp_array[i*scanline_width+j]));
        // inserisco il nuovo pixel appena costruito
        original_pixel_data.push_back(curr_pixel);
        curr_pixel = 0;
      }

      num_nibble++;


    }

    // larghezza immagine non multiplo di 4
    if(width%4 != 0) {
      original_pixel_data.push_back(curr_pixel);
      curr_pixel = 0;
    }
    // ricomincia il conteggio dei nibble dalla nuova scanline
    num_nibble = 1;
    for(int pad = 0; pad < useless_byte; pad++)
      original_pixel_data.push_back(0);
  }

}


void BitmapFile::generate_pixel_array_8(const PaletteIndexer& palette_index, const std::vector<unsigned char>& temp_array) {
  int cont_pixel = 0,
      width = dib_header.width,
      useless_byte = (width % 4 != 0) ? correct_number_byte(width) - width : 0;


  for(int i = 0, pixel_array_size = temp_array.size(); i <= pixel_array_size-2; i+= 3) {
    /*
     A seconda della profondità di colore originale crea un byte array corretto, rispettando
    le modalità di memorizzazione dettate dal formato.
  */
    RgbStruct s(temp_array[i+2], temp_array[i+1], temp_array[i]);
    original_pixel_data.push_back(palette_index.get_index(s));
    cont_pixel++;
    if(cont_pixel == width) {
      for(int i = 0; i < useless_byte; i++)
        original_pixel_data.push_back(0); // zero fill
      cont_pixel = 0;
    }
  }
}

bool BitmapFile::is_dib_header_valid() {
  int nbits = dib_header.bits_per_pixel, comp = 0;

  if (!(nbits == 1 || nbits == 4 || nbits == 8 || nbits == 24 ) ||
      dib_header.planes != 1 || comp > BI_RLE4)
    return false; // immagine non valida
  if (!(comp == BI_RGB || (nbits == 4 && comp == BI_RLE4) ||
        (nbits == 8 && comp == BI_RLE8) ) )
    return false; // tipo di compressione non valido

  return true;
}

/**
  Restituisce
      - TRUE se l'estensione del file risulta essere "bmp"
      - FALSE altrimenti

  */
bool BitmapFile::check_bmp_file(const std::string& file_name) {

  std::string extension = file_name.substr(file_name.find_last_of(".")+1 , file_name.length());
  return (extension == std::string("bmp") || extension == std::string("BMP"));
}

std::string BitmapFile::getInfoBitmap() const {
  std::stringstream stream;

  stream << "<p>Dimensione:                    " << file_header.file_size << "</p>" << std::endl;
  stream << "<p>Tipo file:                     " << file_header.file_type << "</p>" << std::endl;
  stream << "<p>Data offset:                   " << file_header.data_offset << "</p>" << std::endl;
  stream << "<p>Larghezza:                     " << dib_header.width << "</p>" << std::endl;
  stream << "<p>Altezza:                       " << dib_header.height << "</p>" << std::endl;
  stream << "<p>Dimensione dati immagine:      " << dib_header.bitmap_size << "</p>" << std::endl;
  stream << "<p>Bits per pixel:                " << dib_header.bits_per_pixel << "</p>" << std::endl;
  std::string compr_algorithm;

  switch(dib_header.compression) {
    case BI_RLE4:
      compr_algorithm = "RLE_4";
      break;
    case BI_RLE8:
      compr_algorithm = "RLE_8";
      break;
    case BI_RGB:
      compr_algorithm = "";
  }

  if(dib_header.compression != 0)
    stream << "<p>Compressione:                  " << compr_algorithm << "</p>" << std::endl;
  if(dib_header.num_colors)
    stream << "<p>Numero di colori:             " << dib_header.num_colors << "</p>" << std::endl;

  return stream.str();

}
