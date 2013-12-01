#include "BitmapFile.h"
#include <vector>
#include <cstring>
#include "UtilityFunction.h"

BitmapFile::BitmapFile(const std::string& file_name) :
  reversed_pixel_data(std::vector<unsigned char>()),
  original_pixel_data(std::vector<unsigned char>()){
  dib_header.palette = NULL;
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

BitmapFile::~BitmapFile() {

  // dealloco la memoria 'eventualmente' allocata per la palette di colori
  if(dib_header.palette != NULL) {
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

    dib_header.palette = new unsigned char*[num_colors];
    for(int i = 0; i < num_colors; i++) {
      dib_header.palette[i] = new unsigned char[4];
      file_stream.read((char*) dib_header.palette[i], 4);
      //std::cout << "R: " << (int)dib_header.palette[i][2] << " G: " << (int)dib_header.palette[i][1] << " B: " << (int)dib_header.palette[i][0] << std::endl;
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

  if(dib_header.bits_per_pixel == 8)
    file_header.data_offset -= 1024;

  // Controllo che l'immagine non sia stata compressa con un algoritmo di compressione
  // Se fosse stata compressa, provvedo a decomprimerla recuperando i pixel
  /*if(dib_header.compression != 0)
    reversed_pixel_data = decompress_pixel_data();
  */

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
  int tot_size = (dib_header.bitmap_size == 0 ) ? original_pixel_data.size() : dib_header.bitmap_size;
  int height = dib_header.height, width = dib_header.width;

  switch(dib_header.bits_per_pixel) {
    case 24: {
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

	for(std::vector<unsigned char>::const_iterator it = original_pixel_data.begin(),
	    ed = original_pixel_data.end();
	    it != ed;
	    ++it) {
	  // Acquisito il primo carattere, valuto quelle che soni i bit che lo compongono
	  // Se il bit è 0 allora faccio corrispondere ad esso il primo elemento della palette
	  // altrimenti faccio corrispondere a questo il secondo bit della palette

	  // Devo prestare attenzione a non considerare i bit 0 usati come padding per raggiungere
	  // la rappresentazione a 32 bit


	  // genero la rappresentazione a bit del carattere corrente
	  std::vector<bool> curr_char_bits = char_to_bitset(*it);

	  // Effettuo la valutazione per ogni singolo bit generando la tripletta
	  // corrispondente

	  for(std::vector<bool>::const_iterator it_bit = curr_char_bits.begin(),
	      ed_bit = curr_char_bits.end();
	      it_bit != ed_bit && cont_pixel != width;
	      ++it_bit, cont_pixel++) {
	    // se bit è uguale a 0 (rappresentato con il valore booleano 'falso')
	    if(!(*it_bit)) {
	      // inserisco una tripletta con la palette in posizione 0
	      rgb_vect.push_back(RgbStruct(dib_header.palette[0][2], dib_header.palette[0][1], dib_header.palette[0][0]));
	    } else {
	      // inserisco una tripletta con la palette in posizione 1
	      rgb_vect.push_back(RgbStruct(dib_header.palette[1][2], dib_header.palette[1][1], dib_header.palette[1][0]));
	    }
	  }

	  // ho terminato una riga, verifico se ho la necessità di skippare alcuni
	  // bit di padding presenti nel vettore di pixel
	  if(cont_pixel == width) {
	    for(int i = 0; i < useless_byte && it != ed; i++)
	      ++it;
	    cont_pixel = 0;

	  }
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

	std::vector<unsigned char>::const_iterator it = original_pixel_data.begin(),
	    ed = original_pixel_data.end();
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
	int index;

	for(std::vector<unsigned char>::const_iterator it = original_pixel_data.begin(),
	    ed = original_pixel_data.end();
	    it != ed;
	    ++it) {
	  index = *it;
	  vect_rgb.push_back(RgbStruct(dib_header.palette[index][2],dib_header.palette[index][1], dib_header.palette[index][0]));
	}

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

  //print_pixel_data();
}

void BitmapFile::print_pixel_data() {
  for(std::vector<unsigned char>::const_iterator it = original_pixel_data.begin(),
      ed = original_pixel_data.end();
      it != ed;
      ++it)
    std::cout << (int)*it << std::endl;

}

void BitmapFile::print_pixel_data(const std::vector<unsigned char>& vet) {
  for(std::vector<unsigned char>::const_iterator it = vet.begin(),
      ed = vet.end();
      it != ed;
      ++it)
    std::cout << (int)*it << std::endl;

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
      int num_colors = file_header.data_offset - 54;

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
    return NULL;
  }
  return  new BitmapTO(reversed_pixel_data, this->dib_header.width, this->dib_header.height, this->dib_header.bits_per_pixel);
}

/**
  Crea gli header dell'immagine Bitmap che contengono
  le informazioni fondamentali riguardanti l'immagine
*/
void BitmapFile::fill_bmp_header(int width, int height, int bits_per_pixel, int bitmap_size) {
  int num_color_palette = (bits_per_pixel <= 8) ? (pow(2,bits_per_pixel)*4) : 0,
      data_offset = (bits_per_pixel <= 8) ? 54 + num_color_palette : 54;

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
  dib_header.header_size = (bits_per_pixel <= 8) ? 40 * num_color_palette : 40;
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

  switch(dib_header.bits_per_pixel) {
    case 1:
      break;
    case 4:
      break;
    case 8:
      break;
    case 24:
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
      break;
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
