#include "ModifyImageExecutor.h"
#include "ImageOperation.h"
#include "BitmapTO.h"
#include <stdexcept>
#include <iostream>

ResultStruct ModifyImageExecutor::executeOperation(RequestStruct request){
  ImageEffectOperation op = *((ImageEffectOperation*) request.getAttribute(1));
  BitmapTO* to = (BitmapTO*) request.getAttribute(2);

  std::vector<unsigned char> result_array, original_array(to->getPixelData());


  switch(op) {
    case NEGATIVE:
      result_array = imageToNegative(original_array);
      break;
    case GRAYSCALE:
      // viene adoperata la medesima funzione dell'effetto seppia modificando
      // il valore di profondità dell'effetto
      result_array = imageToSepia(original_array, 0);
      break;
    case SEPIA:
      result_array = imageToSepia(original_array, 20);
      break;
    default:
      throw std::invalid_argument("L'effetto scelto non risulta essere supportato");
  }
  to->setPixelData(result_array);
  return ResultStruct(1,to);
}

std::vector<unsigned char> imageToSepia(const std::vector<unsigned char>& pixels, int sepiaDepth) {
    std::vector<unsigned char> modifiedPixels;
    int sepiaIntensity = 5;
    for (size_t i=0, tot_size = pixels.size();i<=tot_size-2; i+=3) {
        int r = pixels[i];
        int g = pixels[i+1];
        int b = pixels[i+2];

        int gry = (r + g + b) / 3;
        r = g = b = gry;
        r = r + (sepiaDepth * 2);
        g = g + sepiaDepth;

        if (r>255) r=255;
        if (g>255) g=255;
        if (b>255) b=255;

        // Fa in modo di rendere più scuro il canale del BLU
        // in modo tale da accentuare l'effetto
        b-= sepiaIntensity;

        // Controlla che i valori ottenuti non vadano fuori dal range
        // dei valori RGB ovvero [0-255]
        if (b<0) b=0;
        if (b>255) b=255;

        // Inserisce la tripletta nel vettore
        modifiedPixels.push_back(r);
        modifiedPixels.push_back(g);
        modifiedPixels.push_back(b);

    }

    return modifiedPixels;
}
std::vector<unsigned char> imageToNegative(const std::vector<unsigned char>& pixels) {
    std::vector<unsigned char> modifiedPixels;
    int r, g, b;

    for(size_t i = 0, tot_size = pixels.size(); i <= tot_size-2; i+=3){
        r = pixels[i];
        g = pixels[i+1];
        b = pixels[i+2];

        // inserisce nel vettore il valore complementare a quello
        // attualmente presente nel vettore
        modifiedPixels.push_back(255 - r);
        modifiedPixels.push_back(255-g);
        modifiedPixels.push_back(255-b);

    }

    return modifiedPixels;
}

