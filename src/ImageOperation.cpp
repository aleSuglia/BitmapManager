#include "ImageOperation.h"

ImageEffectOperation getEffectEnumValue(const std::string& effect_name){
    if("Negative" == effect_name) {
        return NEGATIVE;
    } else if("Grayscale" == effect_name) {
        return GRAYSCALE;
    } else if("Seppia" == effect_name) {
        return SEPIA;
    }

    return NOT_SUPPORTED;

}

std::vector<std::string> getSupportedFilters(){
    std::vector<std::string> filters;

    filters.push_back("Negative");
    filters.push_back("Grayscale");
    filters.push_back("Seppia");

    return filters;

}
