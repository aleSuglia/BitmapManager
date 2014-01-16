#include "PaletteIndexer.h"

PaletteIndexer::PaletteIndexer(unsigned char **palette_char, int num_colors) : palette_index(std::vector<RgbStruct >()) {
  for(int i = 0; i < num_colors; i++) {
    palette_index.push_back(RgbStruct(palette_char[i][2], palette_char[i][1], palette_char[i][0]));
  }

}

PaletteIndexer::PaletteIndexer(const PaletteIndexer& palette) {
  *this = palette;
}

PaletteIndexer& PaletteIndexer::operator=(const PaletteIndexer& palette) {
  if(this != &palette) {
    this->palette_index = palette.palette_index;
  }

  return *this;
}

int PaletteIndexer::get_index(const RgbStruct& rgb) const{
  int index = 0;

  for(std::vector<RgbStruct>::const_iterator it = palette_index.begin(),
      ed = palette_index.end();
      it != ed;
      ++it, ++index)
    if(*it == rgb)
      return index;

  throw std::logic_error("Tripletta RGB non presente nell'indice");
}

