#ifndef BITMAPFILE_H
#define BITMAPFILE_H

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>
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
} DibHeader;

typedef enum {
  RGB3 = 3,
  RGB6 = 6,
  RGB9 = 9,
  RGB12 = 12,
  RGB15 = 15,
  RGB18 = 18,
  RGB24 = 24
} RGB_PALETTE;

typedef enum {
  BI_RGB, //	Formato non compresso
  BI_RLE8, //	Algoritmo RLE per immagini bitmap a 8 bit
  BI_RLE4,//	Algoritmo RLE per immagini bitmap a 4 bit
  BI_BITFIELDS, // Indica che l'immagine non risulta essere compressa e che la tabella di colori consiste di tre maschere di colore per ogni pixel
  BI_JPEG, // Indica che l'immagine è una JPEG
  BI_PNG // Indica che l'immagine è una PNG
} BmpCompression;


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
  void read_bmp_file(const std::string& file_name);


  unsigned char* get_data_array();
  int bitmap_size();
  int height_size();

  int width_size();

  void print_bitmap_information();
  void copy_bmp(const std::string& file_name);  BitmapTO* getTO();

private:
  void fill_bmp_header(int width, int height, int bits_per_pixel, int bitmap_size);

  void retrieve_original_pixel_data();
  bool is_dib_header_valid();
  void fix_pixel_array(char **color_palette = NULL);
  void _swap(unsigned char& a, unsigned char& b);
  std::vector<unsigned char> compress_pixel_data();
  std::vector<unsigned char> decompress_pixel_data();

  bool check_bmp_file(const std::string& file_name);

};



#endif // BITMAPFILE_H
