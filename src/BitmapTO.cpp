#include "BitmapTO.h"

BitmapTO::BitmapTO() : pixelData(std::vector<unsigned char>()), width(0), height(0) {
}

BitmapTO::BitmapTO(std::vector<unsigned char> pixels, int w, int h, int num_bits):
  pixelData(std::vector<unsigned char>(pixels)), width(w), height(h), bits_per_pixel(num_bits) {

}


int BitmapTO::getBitsPerPixel() const
{
  return bits_per_pixel;
}

void BitmapTO::setBitsPerPixel(int value)
{
  bits_per_pixel = value;
}

BitmapTO::BitmapTO(const BitmapTO &to){
  *this = to;
}

BitmapTO& BitmapTO::operator=(const BitmapTO &to){
  if(this != &to) {
    this->width = to.width;
    this->height = to.height;
    this->pixelData = to.pixelData;

  }

  return *this;
}

int BitmapTO::getWidth() const
{
  return width;
}

void BitmapTO::setWidth(int value)
{
  width = value;
}

int BitmapTO::getHeight() const
{
  return height;
}

void BitmapTO::setHeight(int value)
{
  height = value;
}

std::vector<unsigned char> BitmapTO::getPixelData() const
{
  return pixelData;
}

unsigned char *BitmapTO::getPixelDataPtr() {
  return &pixelData[0];
}

void BitmapTO::setPixelData(const std::vector<unsigned char>& value)
{
  pixelData = value;
}

