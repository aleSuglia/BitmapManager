#include "BitmapTO.h"
#include "UtilityFunction.h"

BitmapTO::BitmapTO()
  : pixelData(std::vector<unsigned char>()), aligned_data(0), width(0), height(0), bitmap_size(0), bits_per_pixel(0), info_bitmap(""), compr_mode(0) {
}

BitmapTO::BitmapTO(std::vector<unsigned char> vec, int width, int height, int bitmap_size, int num_bits, std::string info_bitmap, int compr_mode):
  pixelData(std::vector<unsigned char>(vec)), aligned_data(0), width(width), height(height), bitmap_size(bitmap_size),
  bits_per_pixel(num_bits), info_bitmap(info_bitmap), compr_mode(compr_mode) {
  if(width%4 != 0)
    prepare_aligned_data();
}

BitmapTO::~BitmapTO() {
  if(aligned_data != 0)
    delete [] aligned_data;
}

int BitmapTO::getBitmapSize() const {
  return bitmap_size;
}

void BitmapTO::setBitmapSize(int value) {
  bitmap_size = value;
}

int BitmapTO::getBitsPerPixel() const {
  return bits_per_pixel;
}

void BitmapTO::setBitsPerPixel(int value) {
  bits_per_pixel = value;
}

BitmapTO::BitmapTO(const BitmapTO &to) {
  *this = to;
}

BitmapTO& BitmapTO::operator=(const BitmapTO &to){
  if(this != &to) {
    this->width = to.width;
    this->height = to.height;
    this->pixelData = to.pixelData;
    this->compr_mode = to.compr_mode;

    if(width%4 != 0) {
      int padding = correct_number_byte(width*3) - (width*3),
	  tot_dim = pixelData.size() + (height*padding) + 3;
      this->aligned_data = new unsigned char[tot_dim];
      std::copy(this->aligned_data, this->aligned_data+tot_dim, to.aligned_data);
    } else {
      this->aligned_data = 0;
    }
    this->bitmap_size = to.bitmap_size;
  }

  return *this;
}

int BitmapTO::getWidth() const {
  return width;
}

void BitmapTO::setWidth(int value) {
  width = value;
}

int BitmapTO::getHeight() const {
  return height;
}

void BitmapTO::setHeight(int value) {
  height = value;
}

std::vector<unsigned char> BitmapTO::getPixelData() const {
  return pixelData;
}

const char* BitmapTO::getInfoBitmap() const {
  return info_bitmap.c_str();
}

void BitmapTO::setInfoBitmap(const std::string &value) {
  info_bitmap = value;
}


int BitmapTO::getComprMode() const
{
    return compr_mode;
}

void BitmapTO::setComprMode(int value)
{
    compr_mode = value;
}
void BitmapTO::prepare_aligned_data() {
    size_t bytesPerPixel = 3,
            row_len = bytesPerPixel*width, // numero di elementi in una riga dell'immagine
      padding = correct_number_byte(row_len) - row_len, // numero di byte di padding
      array_size = pixelData.size(),
      tot_dim_aligned = array_size + (height*padding),
      aligned_cont = 0;

  aligned_data = new unsigned char[tot_dim_aligned];

  std::fill(aligned_data, aligned_data+tot_dim_aligned, 0);
  std::vector<unsigned char>::iterator it = pixelData.begin();

  for(size_t cont_pixel = 0; cont_pixel < array_size; cont_pixel += row_len) {
    std::copy(it+cont_pixel, it+(cont_pixel+row_len), aligned_data+aligned_cont);
    aligned_cont += row_len + padding; // vado avanti di padding-byte
  }
}

/*
  Restituisce la rappresentazione raw dell'array di
  byte dell'immagine.
  Questo viene restituito apportando un eventuale
  allineamento dei byte per poter garantire un
  allineamento degli stessi a 32-bit.
*/
unsigned char *BitmapTO::getPixelDataPtr() {
  // L'array non necessita di padding se la larghezza dell'immagine
  // e' multiplo di 4
  return (width%4 == 0) ? &pixelData[0] : aligned_data;

  }

  void BitmapTO::setPixelData(const std::vector<unsigned char>& value) {
  pixelData = value;
  if(width%4 != 0)
    prepare_aligned_data();
}
