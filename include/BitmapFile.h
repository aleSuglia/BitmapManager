#ifndef BITMAPFILE_H
#define BITMAPFILE_H

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cmath>
#include "RLECodec.h"
#include "BitmapTO.h"

typedef struct {
  char         file_type[2];   // Valore simbolico settato a "BM"
  unsigned int file_size; // dimensione totale del file
  short        reserved1; // valore riservato fissato a 0
  short        reserved2; // valore riservato fissato a 0
  unsigned int data_offset; // offset necessario per indirizzare il blocco relativo ai dati dell'immagine
} FileHeader;

typedef struct {
  unsigned int header_size; // dimensione dell'header corrente
  int          width; // larghezza dell'immagine
  int          height; // altezza dell'immagine
  short        planes; // numero di piani di colore adoperati
  short        bits_per_pixel;  // profondità del colore (supportate immagini a 24-bit)
  unsigned int compression; // compressione non supportata
  int 		 bitmap_size; // dimensione della parte relativa ai dati (pixel dell'immagine)
  int          horizontal_res; // risoluzione orizzontale dell'immagine
  int          vertical_res; // risoluzione verticale dell'immagine
  unsigned int num_colors; // numero di colori nella palette adoperata
  unsigned int important_colors; // numero di colori rilevanti (0 se sono tutti importanti)
  unsigned char **palette; // palette di colori adoperata dall'immagine (settato unicamente se bit-depth <= 8)

} DibHeader;

/**
    Struttura di supporto per poter individuare
    una tripletta RGB
*/
struct RgbStruct {
public:
  unsigned char r;
  unsigned char g;
  unsigned char b;

  RgbStruct(unsigned char newR, unsigned char newG, unsigned char newB){
    r = newR;
    g = newG;
    b = newB;
  }

};

class BitmapFile {
private:
  FileHeader file_header;
  DibHeader dib_header;
  std::vector<unsigned char> reversed_pixel_data;
  std::vector<unsigned char> original_pixel_data;

public:
  BitmapFile(const std::string& file_name);
  BitmapFile(BitmapTO& to);
  BitmapFile(const BitmapFile& bmp);
  BitmapFile& operator=(const BitmapFile& bmp);
  ~BitmapFile();

  void read_bmp_file(const std::string& file_name);
  unsigned char* get_data_array();
  int bitmap_size();
  int height_size();

  int width_size();

  void print_bitmap_information();
  void copy_bmp(const std::string& file_name);  BitmapTO* getTO();

  class CColor {
  public:
          static inline unsigned int BitCountByMask(unsigned int Mask) {
              unsigned int BitCount = 0;
              while (Mask) {
                  Mask &= Mask - 1;
                  BitCount++;
              }
              return BitCount;
          }

          static inline unsigned int BitPositionByMask(unsigned int Mask) {
              return BitCountByMask((Mask & (~Mask + 1)) - 1);
          }

          static inline unsigned int ComponentByMask(unsigned int Color, unsigned int Mask) {
              unsigned int Component = Color & Mask;
              return Component >> BitPositionByMask(Mask);
          }

          static inline unsigned int BitCountToMask(unsigned int BitCount) {
              return (BitCount == 32) ? 0xFFFFFFFF : (1 << BitCount) - 1;
          }

          static unsigned int Convert(unsigned int Color, unsigned int FromBitCount, unsigned int ToBitCount) {
              if (ToBitCount < FromBitCount) {
                  Color >>= (FromBitCount - ToBitCount);
              } else {
                  Color <<= (ToBitCount - FromBitCount);
                  if (Color > 0) {
                      Color |= BitCountToMask(ToBitCount - FromBitCount);
                  }
              }
              return Color;
          }
      };

private:
  void fill_bmp_header(int width, int height, int bits_per_pixel, int bitmap_size);
  void print_pixel_data();
  void print_pixel_data(const std::vector<unsigned char>&);
  void retrieve_original_pixel_data();
  bool is_dib_header_valid();
  void fix_pixel_array();
  void _swap(unsigned char& a, unsigned char& b);
  std::vector<unsigned char> compress_pixel_data();
  std::vector<unsigned char> decompress_pixel_data();

  bool check_bmp_file(const std::string& file_name);

};



#endif // BITMAPFILE_H
