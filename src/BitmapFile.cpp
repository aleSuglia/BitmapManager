#include "BitmapFile.h"

BitmapFile::BitmapFile(const std::string& file_name) :
  reversed_pixel_data(std::vector<unsigned char>()),
  original_pixel_data(std::vector<unsigned char>()){
  read_bmp_file(file_name);
}

BitmapFile::BitmapFile(BitmapTO& to) :
  reversed_pixel_data(to.getPixelData()),
  original_pixel_data(std::vector<unsigned char>()) {

  fill_bmp_header(to.getWidth(), to.getHeight(), to.getBitsPerPixel(), to.getPixelData().size());
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
  char **palette;
  if(dib_header.num_colors != 0) {
    palette = new char*[dib_header.num_colors];
    for(unsigned int i = 0; i < dib_header.num_colors; i++) {
      palette[i] = new char[4];
      file_stream.read(palette[i], sizeof palette[i]);
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
      Attualmente i pixel risultano essere disposti in verso
      inverso, ovvero ogni pixel viene memorizzato  dal basso(a sinistra) verso l'alto.
      (0,0)B, (0,1)G; (1,0)R, (1,1)W; -> R(1,0)-W(1,1)-B(0,1)-(0,0)G
  */

  // Se l'immagine adopera l'algoritmo di compressio RLE8 effettuo la decompressione
  // ATTENZIONE: Attualmente non implementata

  if(dib_header.bits_per_pixel == 8)
    file_header.data_offset -= 1024;

  // Controllo che l'immagine non sia stata compressa con un algoritmo di compressione
  // Se fosse stata compressa, provvedo a decomprimerla recuperando i pixel
  if(dib_header.compression != 0)
    original_pixel_data = decompress_pixel_data();

  if(dib_header.bits_per_pixel < 16)
    fix_pixel_array(palette);
  else
    fix_pixel_array();

  file_stream.close();

  if(palette != NULL) {
    for(unsigned int i = 0; i < dib_header.num_colors; i++)
      delete [] palette[i];

    delete [] palette;
  }
  print_bitmap_information();


}

void BitmapFile::fix_pixel_array(char **color_palette) {
  /*
      Effettuo una rotazione delle triplette in modo tale da ottenere
      l'array di pixel con le triplette disposte in modo corretto.
      Inizialmente, infatti, le triplette sono disposte nella forma BGR
      di conseguenza ho la necessità di invertirle in modo da ottenere
      la classica tripletta RGB.
  */
  int tot_size = (dib_header.bitmap_size == 0 ) ? original_pixel_data.size() : dib_header.bitmap_size;
  int height = dib_header.height, width = dib_header.width;

  if(dib_header.bits_per_pixel == 24) {
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
  }else if(dib_header.bits_per_pixel == 1) {
    int bit = 128;
    for(std::vector<unsigned char>::const_iterator it = original_pixel_data.begin(),
        ed = original_pixel_data.end();
        it != ed;
        ) {
      if(bit == 128)
        ++it;

      if (*it & bit) {
        reversed_pixel_data.push_back(color_palette[1][2]);
        reversed_pixel_data.push_back(color_palette[1][1]);
        reversed_pixel_data.push_back(color_palette[1][0]);
      } else {
        reversed_pixel_data.push_back(color_palette[0][2]);
        reversed_pixel_data.push_back(color_palette[0][1]);
        reversed_pixel_data.push_back(color_palette[0][0]);
      }

      if (bit > 1)
        bit >>= 1;
      else
        bit = 128;
    }
  }

}

void BitmapFile::_swap(unsigned char& a, unsigned char& b) {
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
  //    for(std::vector<unsigned char>::const_iterator it = original_pixel_data.begin(),
  //        ed = original_pixel_data.end();
  //        it != ed;
  //        ++it)
  //        std::cout << (int) *it << std::endl;

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
    return NULL;
  }

  std::string format;
  switch(this->dib_header.bits_per_pixel){
    case 1:
      format = "RGB1";
      break;
    case 4:
      format = "RGB4";
      break;
    case 8:
      format = "RGB8";
    case 16:
      format = "RGB16";
      break;
    case 24:
      format = "RGB24";
      break;
    case 32:
      format = "RGB32";
      break;
  }

  return  new BitmapTO(reversed_pixel_data, this->dib_header.width, this->dib_header.height, this->dib_header.bits_per_pixel, format);

}

/**
  Crea gli header dell'immagine Bitmap che contengono
  le informazioni fondamentali riguardanti l'immagine
*/
void BitmapFile::fill_bmp_header(int width, int height, int bits_per_pixel, int bitmap_size) {
  // Inizializzo gli header
  memset(&file_header, 0, sizeof(file_header));
  memset(&dib_header, 0, sizeof(dib_header));

  // sigla fissa presente nelle immagini bitmap
  strcpy(file_header.file_type, (const char*)"BM");
  // La dimensione del file è sempre pari alla dimensione dell'array di pixel più la dimensione dei due header
  file_header.file_size = bitmap_size+54;
  // Valori numerici riservati
  file_header.reserved1 = 0;
  file_header.reserved2 = 0;
  // fine valori numerici riservati

  // Offset mediante il quale effettuare l'accesso alla sezione dei dati
  file_header.data_offset = 54;

  // dimensione dell'header corrente
  dib_header.header_size = sizeof(dib_header);
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

}


/**
    Ripristina l'array reversed_pixel_data (che rappresenta i pixel dell'immagine)
    nel formato originale di un immagine di tipo Bitmap
  */
void BitmapFile::retrieve_original_pixel_data() {
  /*
    Effettuo una rotazione delle triplette in modo tale da ottenere
    l'array di pixel con le triplette disposte in modo corretto.
    Inizialmente, infatti, le triplette sono disposte nella forma BGR
    di conseguenza ho la necessità di invertirle in modo da ottenere
    la classica tripletta RGB.
*/
  original_pixel_data = reversed_pixel_data;
  int tot_size = dib_header.bitmap_size;
  int height = dib_header.height, width = dib_header.width;
  for(int i = 0; i <= tot_size-2; i+=3) {
    _swap(original_pixel_data[i], original_pixel_data[i+2]);
  }

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

  for(int i = height-1; i >= 0; i--) {
    for(int j = 0; j < width; j++) {
      original_pixel_data.push_back(rgb_struct[i*width+j].r);
      original_pixel_data.push_back(rgb_struct[i*width+j].g);
      original_pixel_data.push_back(rgb_struct[i*width+j].b);

    }
  }

}



bool BitmapFile::is_dib_header_valid() {
  int nbits = dib_header.bits_per_pixel, comp = 0;

  if (!(nbits == 1 || nbits == 4 || nbits == 8 || nbits == 16 || nbits == 24 || nbits == 32) ||
      dib_header.planes != 1 || comp > BI_BITFIELDS)
    return false; // immagine non valida
  if (!(comp == BI_RGB || (nbits == 4 && comp == BI_RLE4) ||
        (nbits == 8 && comp == BI_RLE8) || ((nbits == 16 || nbits == 32) && comp == BI_BITFIELDS)))
    return false; // tipo di compressione non valido
  return true;
}



std::vector<unsigned char> BitmapFile::compress_pixel_data() {
  /**
  0	Fine della linea
  1	Fine del file bitmap
  2	Delta. I 2 byte che seguono il carattere di escape contengono valori positivi
      che indicano gli offset, orizzontali e verticali, per poter indirizzare il prossimo pixel
      a partire dalla posizione corrente.

  */
  unsigned char prev = original_pixel_data[0];
  std::vector<unsigned char> compressed;

  for(size_t i = 1, count_equal = 0, tot_size = dib_header.bitmap_size; i < tot_size-1; i++) {
    if(original_pixel_data[i] == prev) {
      count_equal++;
    } else {
      compressed.push_back(count_equal);
      compressed.push_back(prev);
      count_equal = 1; // valore trovato la prima volta
    }
  }

  // Inserisce marcatore di fine bitmap
  compressed.push_back(0);
  compressed.push_back(1);

  return compressed;
}

std::vector<unsigned char> BitmapFile::decompress_pixel_data() {
  std::vector<unsigned char> decompress;
  if(original_pixel_data.size() % 2 != 0)
    throw std::logic_error("Impossibile effettuare la decompressione dell'immagine");

  int count, color_index;
  bool end_flag = false;

  for(size_t i = 0, tot_size = original_pixel_data.size(); i <= tot_size-2 && !end_flag; i+=2) {
    count = (int) original_pixel_data[i];
    color_index = (int) original_pixel_data[i+1];

    if( count == 0) {
      switch(color_index) {
        case 1:
          end_flag = true; // EOF trovato
          break;
        case 2: // delta
          break;
      }
    } else {
      for(int count_push = 0; count_push < count; count_push++)
        decompress.push_back(color_index);

    }

  }

  return decompress;
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
