#include "ImageOperation.h"

ImageEffectOperation getEffectEnumValue(const std::string& enum_string){
    if("Negative" == enum_string) {
        return NEGATIVE;
    } else if("Grayscale" == enum_string) {
        return GRAYSCALE;
    } else if("Oil" == enum_string) {
        return OIL;
    } else if("Seppia" == enum_string) {
        return SEPIA;
    }

    return NOT_SUPPORTED;

}

std::vector<std::string> getSupportedFilters(){
    std::vector<std::string> filters;

    filters.push_back("Negative");
    filters.push_back("Grayscale");
    //filters.push_back("Oil");
    filters.push_back("Seppia");

    return filters;

}
