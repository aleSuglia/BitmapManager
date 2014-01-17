#include "BitmapTOwP.h"
#include <cmath>

unsigned char **BitmapTOwP::getPalette() const
{
  return palette;
}

void BitmapTOwP::setPalette(unsigned char **value) {
  palette = value;
}
BitmapTOwP::BitmapTOwP() : palette(0){

}

BitmapTOwP& BitmapTOwP::operator =(const BitmapTOwP& to) {
  if(this != &to) {
    this->bits_per_pixel = to.bits_per_pixel;
    this->width = to.width;
    this->height = to.height;
    this->pixelData = to.pixelData;
    this->aligned_data = to.aligned_data;
    this->bitmap_size = to.bitmap_size;
    this->compr_mode = to.compr_mode;
    int num_colors = pow(2,bits_per_pixel);

    if(to.palette != 0) {
      this->palette = new unsigned char*[num_colors];
      for(int i = 0; i < num_colors; i++) {
        this->palette[i] = new unsigned char[4];
        std::copy(&to.palette[i][0], &to.palette[i][3], &this->palette[i][0]);
      }
    }

  }

  return *this;
}

BitmapTOwP::BitmapTOwP(const BitmapTOwP &to) : BitmapTO(to) {
  *this = to;
}



BitmapTOwP::BitmapTOwP(std::vector<unsigned char> vet, int width, int height, int bitmap_size, int num_bits, unsigned char *palette[4], std::string info_bitmap, int compr_mode) {
  this->pixelData = vet;
  this->width = width;
  this->height = height;
  this->bits_per_pixel = num_bits;
  this->bitmap_size = bitmap_size;
  this->info_bitmap = info_bitmap;
  this->compr_mode = compr_mode;

  if(width% 4 != 0)
    prepare_aligned_data();
  int num_colors = pow(2,num_bits);

  if(palette != 0) {
    this->palette = new unsigned char*[num_colors];
    for(int i = 0; i < num_colors; i++) {
      this->palette[i] = new unsigned char[4];
      std::copy(&palette[i][0], &palette[i][3], &this->palette[i][0]);
    }

  }

}

BitmapTOwP::~BitmapTOwP() {
  if(this->palette != 0) {
    int num_colors = pow(2,this->bits_per_pixel);
    for(int i = 0; i < num_colors; i++)
      delete [] this->palette[i];
    delete [] this->palette;
  }
}
