#ifndef BITMAPTO_H
#define BITMAPTO_H

#include <string>
#include <vector>
#include <QImage>

class BitmapTO
{
private:
    std::vector<unsigned char> pixelData;
    int width;
    int height;
    int bits_per_pixel;
    QImage::Format format;
    QImage::Format convertFormat(const std::string& format);
public:
    BitmapTO();
    BitmapTO(std::vector<unsigned char>, int, int, int num_bits, const std::string&);
    BitmapTO(const BitmapTO& to);
    BitmapTO& operator=(const BitmapTO& to);
    int getWidth() const;
    void setWidth(int value);
    int getHeight() const;
    void setHeight(int value);
    QImage::Format getFormat() const;
    void setFormat(const std::string &value);
    std::vector<unsigned char> getPixelData() const;
    unsigned char* getPixelDataPtr();
    void setPixelData(const std::vector<unsigned char> &value);
    int getBitsPerPixel() const;
    void setBitsPerPixel(int value);
};

#endif // BITMAPTO_H
