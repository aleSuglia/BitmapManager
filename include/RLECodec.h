#ifndef RLECODEC_H
#define RLECODEC_H

#include <vector>

typedef enum {
  BI_RGB, //	Formato non compresso
  BI_RLE8, //	Algoritmo RLE per immagini bitmap a 8 bit
  BI_RLE4 //	Algoritmo RLE per immagini bitmap a 4 bit
} BmpCompression;


class RLECodec
{
public:
  static std::vector<unsigned char> decode_array(const std::vector<unsigned char>&, BmpCompression);
  static std::vector<unsigned char> encode_array(const std::vector<unsigned char>&, BmpCompression);
};

#endif // RLECODEC_H
