#include "BitmapTO.h"

BitmapTO::BitmapTO() : pixelData(std::vector<unsigned char>()), width(0), height(0), format()
{
}

BitmapTO::BitmapTO(std::vector<unsigned char> pixels, int w, int h, int num_bits, const std::string &format):
  pixelData(std::vector<unsigned char>(pixels)), width(w), height(h), bits_per_pixel(num_bits),format(convertFormat(format)) {

}


int BitmapTO::getBitsPerPixel() const
{
  return bits_per_pixel;
}

void BitmapTO::setBitsPerPixel(int value)
{
  bits_per_pixel = value;
}
QImage::Format BitmapTO::convertFormat(const std::string& value){

  if(value == "RGB1")
    return QImage::Format_Mono;
  else if(value == "RGB16")
    return QImage::Format_RGB16;
  else if( value == "RGB24")
   return QImage::Format_RGB888;
  else if(value == "RGB32")
    return QImage::Format_RGB32;
  else
    return QImage::Format_Indexed8;

}

BitmapTO::BitmapTO(const BitmapTO &to){
  *this = to;
}

BitmapTO& BitmapTO::operator=(const BitmapTO &to){
  if(this != &to) {
    this->width = to.width;
    this->height = to.height;
    this->format = to.format;

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

QImage::Format BitmapTO::getFormat() const
{
  return format;
}

void BitmapTO::setFormat(const std::string &value)
{
  if(value == "RGB1")
    this->format = QImage::Format_Mono;
  else if(value == "RGB16")
    this->format = QImage::Format_RGB16;
  else if( value == "RGB24")
    this->format = QImage::Format_RGB888;
  else if(value == "RGB32")
    this->format = QImage::Format_RGB32;
  else
    this->format = QImage::Format_Indexed8;

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

