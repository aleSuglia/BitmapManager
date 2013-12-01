#ifndef IMAGEOPERATION_H
#define IMAGEOPERATION_H

#include <vector>
#include <string>

typedef enum{
  OPEN_IMAGE,
  SAVE_IMAGE,
  MODIFY_IMAGE
}ImageOperation;

typedef enum {
    NOT_SUPPORTED,
    NEGATIVE,
    GRAYSCALE,
    OIL,
    SEPIA
}ImageEffectOperation;

ImageEffectOperation getEffectEnumValue(const std::string& str);
std::vector<std::string> getSupportedFilters();



#endif // IMAGEOPERATION_H
